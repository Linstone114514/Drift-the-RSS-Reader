#include "core/storage/ArticleStore.h"
#include "core/storage/JsonStore.h"
#include "core/domain/ArticleKey.h"
#include "platform/PlatformInfo.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QUuid>
#include <QSet>
#include <QCoreApplication>

namespace drift {

ArticleStore::ArticleStore(QObject *parent)
    : QObject(parent)
{
    m_starSaveTimer.setSingleShot(true);
    m_starSaveTimer.setInterval(500);
    connect(&m_starSaveTimer, &QTimer::timeout, this, &ArticleStore::saveStars);
    // The debounce timer never fires once the event loop stops; flush at quit.
    connect(qApp, &QCoreApplication::aboutToQuit, this, [this] {
        if (m_starSaveTimer.isActive()) {
            m_starSaveTimer.stop();
            saveStars();
        }
    });
}

void ArticleStore::load()
{
    // Load star index first
    loadStars();

    // Shards are loaded lazily as feeds are selected
    emit storeLoaded();
}

void ArticleStore::loadFeed(const QString &feedId)
{
    if (m_articles.contains(feedId))
        return;

    const QJsonObject obj = JsonStore::loadJson(feedShardPath(feedId));
    if (obj.isEmpty()) {
        m_articles.insert(feedId, {});
        return;
    }

    QList<Article> articles;
    const QJsonArray arr = obj.value(QStringLiteral("articles")).toArray();
    for (const auto &v : arr) {
        Article a = Article::fromJson(v.toObject());
        // Restore star state from star index
        if (m_stars.value(feedId).contains(a.id))
            a.isStarred = true;
        articles.append(std::move(a));
    }

    // Sort by published descending
    std::sort(articles.begin(), articles.end(),
              [](const Article &a, const Article &b) {
                  return a.published > b.published;
              });

    m_articles.insert(feedId, articles);

    // Build dedup index for this feed
    for (const auto &a : articles) {
        const QString key = articleKey(a);
        if (!key.isEmpty())
            m_dedupKeys.insert(key, a.id);
    }
}

QList<QString> ArticleStore::upsertFeedArticles(const QString &feedId,
                                                  const QList<Article> &incoming)
{
    // Ensure feed shard is loaded
    QList<Article> &existing = m_articles[feedId];
    QHash<QString, int> existingIndex; // articleId -> index in existing list
    for (int i = 0; i < existing.size(); ++i)
        existingIndex.insert(existing[i].id, i);

    QStringList newIds;
    QSet<QString> incomingKeys; // dedupe within this batch

    // Make a mutable copy since we need to modify fields
    QList<Article> mutableIncoming = incoming;

    for (auto &article : mutableIncoming) {
        const QString key = articleKey(article);
        if (key.isEmpty()) continue;

        // Skip duplicates within the incoming batch itself
        if (incomingKeys.contains(key)) continue;
        incomingKeys.insert(key);

        // Check existing articles
        if (m_dedupKeys.contains(key)) {
            const QString existingId = m_dedupKeys.value(key);
            if (existingIndex.contains(existingId)) {
                // Update existing article metadata (but preserve read/starred state)
                Article &old = existing[existingIndex.value(existingId)];
                const bool wasRead = old.isRead;
                const bool wasStarred = old.isStarred;
                old = article;
                old.id = existingId; // Keep original ID
                old.isRead = wasRead;
                old.isStarred = wasStarred;
                continue;
            }
        }

        // New article
        if (article.id.isEmpty())
            article.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
        article.feedId = feedId;
        article.fetched = QDateTime::currentDateTimeUtc();

        // Apply star state from star index
        if (m_stars.value(feedId).contains(article.id))
            article.isStarred = true;

        m_dedupKeys.insert(key, article.id);
        existing.append(article);
        newIds.append(article.id);
    }

    // Re-sort by published descending
    std::sort(existing.begin(), existing.end(),
              [](const Article &a, const Article &b) {
                  return a.published > b.published;
              });

    // Persist
    saveFeedShard(feedId);

    if (!newIds.isEmpty())
        emit articlesAdded(feedId, newIds);

    return newIds;
}

void ArticleStore::clearContentCached(const QString &articleId)
{
    for (auto it = m_articles.begin(); it != m_articles.end(); ++it) {
        for (auto &a : it.value()) {
            if (a.id == articleId) {
                a.contentCached = false;
                a.contentHash.clear();
                saveFeedShard(it.key());
                return;
            }
        }
    }
}

void ArticleStore::setRead(const QStringList &articleIds, bool read)
{
    QSet<QString> dirtyFeeds;
    for (const auto &id : articleIds) {
        for (auto it = m_articles.begin(); it != m_articles.end(); ++it) {
            for (auto &a : it.value()) {
                if (a.id == id) {
                    a.isRead = read;
                    dirtyFeeds.insert(it.key());
                    break;
                }
            }
        }
    }
    // Persist only the shards that actually changed
    for (const auto &feedId : std::as_const(dirtyFeeds))
        saveFeedShard(feedId);

    emit articleFlagsChanged(articleIds);
}

void ArticleStore::setStarred(const QStringList &articleIds, bool starred)
{
    QSet<QString> dirtyFeeds;
    for (const auto &id : articleIds) {
        for (auto it = m_articles.begin(); it != m_articles.end(); ++it) {
            for (auto &a : it.value()) {
                if (a.id == id) {
                    a.isStarred = starred;
                    // Update star index
                    if (starred)
                        m_stars[it.key()].insert(id);
                    else
                        m_stars[it.key()].remove(id);
                    dirtyFeeds.insert(it.key());
                    break;
                }
            }
        }
    }
    m_starSaveTimer.start();
    for (const auto &feedId : std::as_const(dirtyFeeds))
        saveFeedShard(feedId);

    emit articleFlagsChanged(articleIds);
}

void ArticleStore::markFeedRead(const QString &feedId)
{
    loadFeed(feedId);
    QStringList ids;
    auto &articles = m_articles[feedId];
    for (auto &a : articles) {
        if (!a.isRead) {
            a.isRead = true;
            ids.append(a.id);
        }
    }
    saveFeedShard(feedId);
    if (!ids.isEmpty())
        emit articleFlagsChanged(ids);
}

void ArticleStore::markAllRead()
{
    QStringList ids;
    for (auto it = m_articles.begin(); it != m_articles.end(); ++it) {
        for (auto &a : it.value()) {
            if (!a.isRead) {
                a.isRead = true;
                ids.append(a.id);
            }
        }
        saveFeedShard(it.key());
    }
    if (!ids.isEmpty())
        emit articleFlagsChanged(ids);
}

QList<Article> ArticleStore::articlesForFeed(const QString &feedId) const
{
    return m_articles.value(feedId);
}

QList<Article> ArticleStore::starredArticles() const
{
    QList<Article> result;
    for (auto it = m_articles.cbegin(); it != m_articles.cend(); ++it) {
        const QSet<QString> &starredIds = m_stars.value(it.key());
        for (const auto &a : it.value()) {
            if (starredIds.contains(a.id))
                result.append(a);
        }
    }
    std::sort(result.begin(), result.end(),
              [](const Article &a, const Article &b) { return a.published > b.published; });
    return result;
}

QList<Article> ArticleStore::unreadArticles() const
{
    QList<Article> result;
    for (const auto &articles : m_articles) {
        for (const auto &a : articles) {
            if (!a.isRead)
                result.append(a);
        }
    }
    std::sort(result.begin(), result.end(),
              [](const Article &a, const Article &b) { return a.published > b.published; });
    return result;
}

QList<Article> ArticleStore::recentArticles(int days) const
{
    const QDateTime cutoff = QDateTime::currentDateTimeUtc().addDays(-days);
    QList<Article> result;
    for (const auto &articles : m_articles) {
        for (const auto &a : articles) {
            if (a.published >= cutoff)
                result.append(a);
        }
    }
    std::sort(result.begin(), result.end(),
              [](const Article &a, const Article &b) { return a.published > b.published; });
    return result;
}

QList<Article> ArticleStore::searchArticles(const QString &query) const
{
    QList<Article> result;
    const QString q = query.toLower();
    for (const auto &articles : m_articles) {
        for (const auto &a : articles) {
            if (a.title.toLower().contains(q) ||
                a.summary.toLower().contains(q) ||
                a.author.toLower().contains(q)) {
                result.append(a);
            }
        }
    }
    return result;
}

Article ArticleStore::articleById(const QString &id) const
{
    for (const auto &articles : m_articles) {
        for (const auto &a : articles) {
            if (a.id == id)
                return a;
        }
    }
    return {};
}

int ArticleStore::unreadCount(const QString &feedId) const
{
    int count = 0;
    const auto articles = m_articles.value(feedId);
    for (const auto &a : articles) {
        if (!a.isRead) ++count;
    }
    return count;
}

int ArticleStore::totalUnreadCount() const
{
    int count = 0;
    for (const auto &articles : m_articles) {
        for (const auto &a : articles) {
            if (!a.isRead) ++count;
        }
    }
    return count;
}

void ArticleStore::purgeOld(int maxDays, int maxPerFeed)
{
    const QDateTime cutoff = QDateTime::currentDateTimeUtc().addDays(-maxDays);

    for (auto it = m_articles.begin(); it != m_articles.end(); ++it) {
        QList<Article> &articles = it.value();
        // Remove articles older than cutoff
        articles.erase(std::remove_if(articles.begin(), articles.end(),
            [&](const Article &a) {
                return a.published < cutoff && !a.isStarred;
            }), articles.end());

        // Cap at maxPerFeed (remove oldest first)
        if (articles.size() > maxPerFeed) {
            // Sort by published descending
            std::sort(articles.begin(), articles.end(),
                      [](const Article &a, const Article &b) { return a.published > b.published; });
            articles.erase(articles.begin() + maxPerFeed, articles.end());
        }

        saveFeedShard(it.key());
    }
}

// ── Private helpers ────────────────────────────────────────────────────

QString ArticleStore::feedShardPath(const QString &feedId) const
{
    return PlatformInfo::dataRoot() + QStringLiteral("/articles/")
           + feedId + QStringLiteral(".json");
}

QString ArticleStore::starsPath() const
{
    return PlatformInfo::dataRoot() + QStringLiteral("/stars.json");
}

QString ArticleStore::articleKey(const Article &a) const
{
    return drift::articleKey(a.guid, a.link, a.title,
                              a.published.toString(Qt::ISODate));
}

void ArticleStore::loadStars()
{
    const QJsonObject obj = JsonStore::loadJson(starsPath());
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        const QJsonArray arr = it.value().toArray();
        QSet<QString> ids;
        for (const auto &v : arr)
            ids.insert(v.toString());
        m_stars.insert(it.key(), ids);
    }
}

void ArticleStore::saveStars()
{
    QJsonObject obj;
    for (auto it = m_stars.cbegin(); it != m_stars.cend(); ++it) {
        QJsonArray arr;
        for (const auto &id : it.value())
            arr.append(id);
        obj[it.key()] = arr;
    }
    JsonStore::saveJson(obj, starsPath());
}

void ArticleStore::saveFeedShard(const QString &feedId)
{
    const auto &articles = m_articles.value(feedId);
    QJsonObject obj;
    obj[QStringLiteral("version")] = 1;
    obj[QStringLiteral("feedId")] = feedId;

    QJsonArray arr;
    for (const auto &a : articles)
        arr.append(a.toJson());
    obj[QStringLiteral("articles")] = arr;

    JsonStore::saveJson(obj, feedShardPath(feedId));
}

} // namespace drift
