#include "DriftController.h"
#include "core/feed/FeedParser.h"
#include "core/storage/OpmlService.h"
#include <QUuid>
#include <QDesktopServices>
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include <QCryptographicHash>

namespace drift {

DriftController::DriftController(QObject *parent)
    : QObject(parent)
{
}

void DriftController::initialize()
{
    // Create services
    m_settingsStore = new SettingsStore(this);
    m_subscriptionStore = new SubscriptionStore(this);
    m_articleStore = new ArticleStore(this);
    m_feedFetcher = new FeedFetcher(this);
    m_faviconFetcher = new FaviconFetcher(this);
    m_contentCache = new ContentCache(this);
    m_htmlConverter = new HtmlConverter(this);
    m_offlineManager = new OfflineManager(this);

    // Create models
    m_feedTreeModel = new FeedTreeModel(m_subscriptionStore, m_articleStore, this);
    m_articleListModel = new ArticleListModel(m_articleStore, m_subscriptionStore, this);

    // Wire signals
    connect(m_feedFetcher, &FeedFetcher::feedDownloaded,
            this, &DriftController::onFeedDownloaded);
    connect(m_feedFetcher, &FeedFetcher::feedNotModified,
            this, &DriftController::onFeedNotModified);
    connect(m_feedFetcher, &FeedFetcher::feedFailed,
            this, &DriftController::onFeedFailed);
    connect(m_feedFetcher, &FeedFetcher::progressChanged,
            this, &DriftController::onRefreshProgress);
    connect(m_feedFetcher, &FeedFetcher::allDone,
            this, &DriftController::onAllDone);

    connect(m_faviconFetcher, &FaviconFetcher::iconReady,
            this, &DriftController::onFaviconReady);

    connect(m_offlineManager, &OfflineManager::offlineChanged,
            this, &DriftController::offlineChanged);

    // Load persisted data
    m_settingsStore->load();
    m_subscriptionStore->load();
    m_articleStore->load();

    // Refresh on startup if configured
    if (m_settingsStore->refreshOnStartup()) {
        QTimer::singleShot(500, this, &DriftController::refreshAll);
    }
}

// ── Slots ──────────────────────────────────────────────────────────────

void DriftController::addFeed(const QString &url)
{
    const QUrl feedUrl(url);
    if (!feedUrl.isValid()) {
        emit notification(tr("Invalid URL"), tr("The feed URL is not valid."));
        return;
    }

    // Check for duplicates
    Feed existing = m_subscriptionStore->feedByUrl(url);
    if (!existing.id.isEmpty()) {
        emit notification(tr("Already Subscribed"),
                          tr("You are already subscribed to this feed."));
        return;
    }

    // Create a placeholder feed and fetch it
    Feed feed;
    feed.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    feed.title = url; // Temp title, will be updated on fetch
    feed.url = url;
    feed.refreshIntervalMinutes = m_settingsStore->refreshIntervalMinutes();

    // Start fetching — feed will be added on successful parse
    m_feedFetcher->fetch(feed);

    // Add to subscriptions immediately so it appears in the tree
    m_subscriptionStore->addFeed(feed);

    // Start favicon discovery
    m_faviconFetcher->fetchIcon(feed.id, {}, url);
}

void DriftController::removeFeed(const QString &feedId)
{
    m_subscriptionStore->removeFeed(feedId);
}

void DriftController::refreshAll()
{
    const auto feeds = m_subscriptionStore->feeds();
    m_refreshDone = 0;
    m_refreshTotal = feeds.size();
    emit refreshProgressChanged();
    emit isRefreshingChanged();
    m_feedFetcher->refreshAll(feeds);
}

void DriftController::refreshFeed(const QString &feedId)
{
    Feed feed = m_subscriptionStore->feedById(feedId);
    if (!feed.id.isEmpty()) {
        m_refreshDone = 0;
        m_refreshTotal = 1;
        emit refreshProgressChanged();
        emit isRefreshingChanged();
        m_feedFetcher->fetch(feed);
    }
}

void DriftController::markRead(const QStringList &articleIds, bool read)
{
    m_articleStore->setRead(articleIds, read);
}

void DriftController::markAllRead()
{
    m_articleStore->markAllRead();
}

void DriftController::markFeedRead(const QString &feedId)
{
    m_articleStore->markFeedRead(feedId);
}

void DriftController::toggleStar(const QString &articleId)
{
    Article article = m_articleStore->articleById(articleId);
    if (!article.id.isEmpty()) {
        m_articleStore->setStarred({articleId}, !article.isStarred);
    }
}

void DriftController::setView(int view, const QString &feedId)
{
    m_articleListModel->setView(view, feedId);
}

QString DriftController::readerContent(const QString &articleId)
{
    Article article = m_articleStore->articleById(articleId);
    if (article.id.isEmpty())
        return {};

    if (m_contentCache->hasCached(articleId)) {
        // Use cached HTML and convert to markdown
        QFile file(m_contentCache->cachedContentPath(articleId));
        if (file.open(QIODevice::ReadOnly)) {
            const QString html = QString::fromUtf8(file.readAll());
            return m_htmlConverter->toMarkdown(html, QUrl(article.link), articleId);
        }
    }

    // No cache — mark as read and return summary
    if (!article.isRead)
        markRead({articleId});

    return QStringLiteral("## %1\n\n%2\n\n[%3](%4)")
        .arg(article.title, article.summary, tr("Read original"), article.link);
}

void DriftController::openOriginal(const QString &url)
{
    QDesktopServices::openUrl(QUrl(url));
}

QString DriftController::exportOpml(const QString &filePath)
{
    const bool ok = OpmlService::exportOpml(filePath,
        m_subscriptionStore->feeds(), m_subscriptionStore->categories());
    return ok ? QString() : tr("Failed to export OPML file.");
}

QString DriftController::importOpml(const QString &filePath)
{
    const auto outlines = OpmlService::importOpml(filePath);
    if (outlines.isEmpty())
        return tr("No feeds found in the OPML file.");

    int added = 0;
    QStringList errors;

    for (const auto &outline : outlines) {
        // Check duplicate
        if (!m_subscriptionStore->feedByUrl(outline.xmlUrl).id.isEmpty())
            continue;

        Feed feed;
        feed.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
        feed.title = outline.title.isEmpty() ? outline.xmlUrl : outline.title;
        feed.url = outline.xmlUrl;
        feed.siteUrl = outline.htmlUrl;

        // Check if we need to create a category
        if (!outline.categoryName.isEmpty()) {
            // Find or create category
            bool found = false;
            for (const auto &cat : m_subscriptionStore->categories()) {
                if (cat.name == outline.categoryName) {
                    feed.categoryId = cat.id;
                    found = true;
                    break;
                }
            }
            if (!found) {
                Category cat;
                cat.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
                cat.name = outline.categoryName;
                m_subscriptionStore->addCategory(cat);
                feed.categoryId = cat.id;
            }
        }

        m_subscriptionStore->addFeed(feed);
        m_faviconFetcher->fetchIcon(feed.id, outline.htmlUrl, outline.xmlUrl);
        ++added;
    }

    // Trigger refresh for all newly added feeds
    refreshAll();

    return tr("Imported %1 feeds successfully.").arg(added);
}

void DriftController::saveWindowGeometry(int width, int height)
{
    m_settingsStore->setWindowWidth(width);
    m_settingsStore->setWindowHeight(height);
}

// ── Private slots ──────────────────────────────────────────────────────

void DriftController::onFeedDownloaded(const QString &feedId, const QByteArray &body,
                                         const QString &etag, const QString &lastModified)
{
    // Parse the feed
    Feed feed = m_subscriptionStore->feedById(feedId);
    if (feed.id.isEmpty()) return;

    const FeedParseResult result = FeedParser::parse(body, QUrl(feed.url));

    if (!result.error.isEmpty()) {
        feed.lastError = tr("Parse error: ") + result.error;
        feed.lastFetched = QDateTime::currentDateTimeUtc();
        m_subscriptionStore->updateFeed(feed);
        return;
    }

    // Update feed metadata
    feed.title = result.meta.title.isEmpty() ? feed.title : result.meta.title;
    feed.description = result.meta.description.isEmpty()
        ? feed.description : result.meta.description;
    if (!result.meta.siteUrl.isEmpty())
        feed.siteUrl = result.meta.siteUrl;
    feed.etag = etag;
    feed.lastModified = lastModified;
    feed.lastFetched = QDateTime::currentDateTimeUtc();
    feed.lastError.clear();
    m_subscriptionStore->updateFeed(feed);

    // Convert parsed items to articles
    QList<Article> articles;
    articles.reserve(result.items.size());
    for (const auto &item : result.items) {
        Article a;
        a.feedId = feedId;
        a.guid = item.guid;
        a.title = item.title;
        a.author = item.author;
        a.link = item.link;
        a.summary = item.summary;
        a.published = item.published.isValid() ? item.published : QDateTime::currentDateTimeUtc();
        a.updated = item.updated;
        a.hasEnclosure = item.hasEnclosure;
        a.thumbPath = item.thumbUrl;

        // Generate summary if not provided
        if (a.summary.isEmpty() && !item.contentHtml.isEmpty()) {
            a.summary = m_htmlConverter->toPlainText(item.contentHtml);
        }

        // Cache the content
        if (!item.contentHtml.isEmpty()) {
            const QByteArray htmlBytes = item.contentHtml.toUtf8();
            a.contentHash = QString::fromLatin1(
                QCryptographicHash::hash(htmlBytes, QCryptographicHash::Sha1).toHex());
            m_contentCache->cacheArticleContent(a.id.isEmpty()
                ? QUuid::createUuid().toString(QUuid::WithoutBraces) : a.id, htmlBytes);
            a.contentCached = true;
        }

        articles.append(std::move(a));
    }

    // Upsert into article store
    m_articleStore->loadFeed(feedId);
    m_articleStore->upsertFeedArticles(feedId, articles);

    // Start favicon discovery if we have a site URL now
    if (!feed.siteUrl.isEmpty())
        m_faviconFetcher->fetchIcon(feedId, feed.siteUrl, feed.url);
}

void DriftController::onFeedNotModified(const QString &feedId)
{
    Feed feed = m_subscriptionStore->feedById(feedId);
    if (feed.id.isEmpty()) return;

    feed.lastFetched = QDateTime::currentDateTimeUtc();
    feed.lastError.clear();
    m_subscriptionStore->updateFeed(feed);
}

void DriftController::onFeedFailed(const QString &feedId, const QString &error)
{
    Feed feed = m_subscriptionStore->feedById(feedId);
    if (feed.id.isEmpty()) return;

    feed.lastError = error;
    feed.lastFetched = QDateTime::currentDateTimeUtc();
    m_subscriptionStore->updateFeed(feed);
}

void DriftController::onRefreshProgress(int done, int total)
{
    m_refreshDone = done;
    m_refreshTotal = total;
    emit refreshProgressChanged();
}

void DriftController::onAllDone()
{
    emit isRefreshingChanged();
}

void DriftController::onFaviconReady(const QString &feedId, const QString &localPath)
{
    Feed feed = m_subscriptionStore->feedById(feedId);
    if (feed.id.isEmpty()) return;

    feed.iconPath = localPath;
    m_subscriptionStore->updateFeed(feed);
}

bool DriftController::isRefreshing() const
{
    return m_feedFetcher && m_feedFetcher->isRefreshing();
}

bool DriftController::isOffline() const
{
    return m_offlineManager && m_offlineManager->isOffline();
}

} // namespace drift
