#include "core/models/ArticleListModel.h"
#include "core/storage/ArticleStore.h"
#include "core/storage/SubscriptionStore.h"
#include <QDateTime>
#include <QLocale>

namespace drift {

ArticleListModel::ArticleListModel(ArticleStore *artStore, SubscriptionStore *subStore,
                                     QObject *parent)
    : QAbstractListModel(parent)
    , m_artStore(artStore)
    , m_subStore(subStore)
{
    connect(m_artStore, &ArticleStore::articlesAdded,
            this, &ArticleListModel::onArticlesAdded);
    connect(m_artStore, &ArticleStore::articleFlagsChanged,
            this, &ArticleListModel::onArticleFlagsChanged);
}

void ArticleListModel::setView(int view, const QString &feedId)
{
    m_view = static_cast<View>(view);
    if (view == FeedView)
        m_feedId = feedId;
    else
        m_feedId.clear();
    reload();
}

void ArticleListModel::setSearchQuery(const QString &query)
{
    if (m_searchQuery != query) {
        m_searchQuery = query;
        applyFilters();
        emit searchQueryChanged();
    }
}

QString ArticleListModel::searchQuery() const
{
    return m_searchQuery;
}

void ArticleListModel::setShowUnreadOnly(bool unreadOnly)
{
    if (m_showUnreadOnly != unreadOnly) {
        m_showUnreadOnly = unreadOnly;
        applyFilters();
    }
}

QVariantMap ArticleListModel::get(int row) const
{
    QVariantMap map;
    if (row < 0 || row >= m_articles.size())
        return map;

    const auto &a = m_articles[row];
    map[QStringLiteral("id")] = a.id;
    map[QStringLiteral("feedId")] = a.feedId;
    map[QStringLiteral("title")] = a.title;
    map[QStringLiteral("summary")] = a.summary;
    map[QStringLiteral("author")] = a.author;
    map[QStringLiteral("published")] = a.published;
    map[QStringLiteral("link")] = a.link;
    map[QStringLiteral("isRead")] = a.isRead;
    map[QStringLiteral("isStarred")] = a.isStarred;
    map[QStringLiteral("thumbPath")] = a.thumbPath;
    map[QStringLiteral("contentCached")] = a.contentCached;
    return map;
}

int ArticleListModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_articles.size();
}

QVariant ArticleListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_articles.size())
        return {};

    const auto &a = m_articles[index.row()];
    const Feed feed = m_subStore->feedById(a.feedId);

    switch (role) {
    case ArticleIdRole:    return a.id;
    case FeedIdRole:       return a.feedId;
    case TitleRole:        return a.title;
    case SummaryRole:      return a.summary;
    case AuthorRole:       return a.author;
    case PublishedRole:    return a.published;
    case PublishedRelativeRole: {
        const qint64 secs = a.published.secsTo(QDateTime::currentDateTimeUtc());
        if (secs < 3600) return tr("%1m ago").arg(secs / 60);
        if (secs < 86400) return tr("%1h ago").arg(secs / 3600);
        if (secs < 604800) return tr("%1d ago").arg(secs / 86400);
        return QLocale::system().toString(a.published.toLocalTime(), QLocale::ShortFormat);
    }
    case FeedNameRole:     return feed.title;
    case FeedIconRole:     return feed.iconPath;
    case IsReadRole:       return a.isRead;
    case IsStarredRole:    return a.isStarred;
    case ThumbPathRole:    return a.thumbPath;
    case HasThumbRole:     return !a.thumbPath.isEmpty();
    case ContentCachedRole: return a.contentCached;
    case LinkRole:          return a.link;
    default: break;
    }

    return {};
}

QHash<int, QByteArray> ArticleListModel::roleNames() const
{
    return {
        { ArticleIdRole, "articleId" },
        { FeedIdRole, "feedId" },
        { TitleRole, "title" },
        { SummaryRole, "summary" },
        { AuthorRole, "author" },
        { PublishedRole, "published" },
        { PublishedRelativeRole, "publishedRelative" },
        { FeedNameRole, "feedName" },
        { FeedIconRole, "feedIcon" },
        { IsReadRole, "isRead" },
        { IsStarredRole, "isStarred" },
        { ThumbPathRole, "thumbPath" },
        { HasThumbRole, "hasThumb" },
        { ContentCachedRole, "contentCached" },
        { LinkRole, "link" }
    };
}

// ── Private slots ──────────────────────────────────────────────────────

void ArticleListModel::onArticlesAdded(const QString &feedId, const QStringList &newIds)
{
    Q_UNUSED(feedId)
    Q_UNUSED(newIds)
    reload();
}

void ArticleListModel::onArticleFlagsChanged(const QStringList &ids)
{
    // Update affected rows
    for (const auto &id : ids) {
        if (m_indexMap.contains(id)) {
            const int row = m_indexMap[id];
            emit dataChanged(index(row), index(row), {IsReadRole, IsStarredRole});
        }
    }
    // If showing unread-only view, reapplying filters may be needed
    if (m_showUnreadOnly)
        applyFilters();
}

void ArticleListModel::reload()
{
    // Collect source articles based on current view
    m_allArticles.clear();

    switch (m_view) {
    case AllView:
        for (const auto &feed : m_subStore->feeds()) {
            m_artStore->loadFeed(feed.id);
            m_allArticles.append(m_artStore->articlesForFeed(feed.id));
        }
        break;
    case UnreadView:
        for (const auto &feed : m_subStore->feeds()) {
            m_artStore->loadFeed(feed.id);
            for (const auto &a : m_artStore->articlesForFeed(feed.id)) {
                if (!a.isRead)
                    m_allArticles.append(a);
            }
        }
        break;
    case StarredView:
        m_allArticles = m_artStore->starredArticles();
        break;
    case TodayView:
        m_allArticles = m_artStore->recentArticles(1);
        break;
    case FeedView:
        m_artStore->loadFeed(m_feedId);
        m_allArticles = m_artStore->articlesForFeed(m_feedId);
        break;
    }

    // Sort by published descending
    std::sort(m_allArticles.begin(), m_allArticles.end(),
              [](const Article &a, const Article &b) {
                  return a.published > b.published;
              });

    // applyFilters handles beginResetModel/endResetModel
    applyFilters();
}

void ArticleListModel::applyFilters()
{
    beginResetModel();
    m_articles.clear();
    m_indexMap.clear();

    const QString query = m_searchQuery.toLower();

    for (const auto &a : m_allArticles) {
        if (m_showUnreadOnly && a.isRead)
            continue;
        if (!query.isEmpty()) {
            if (!a.title.toLower().contains(query) &&
                !a.summary.toLower().contains(query) &&
                !a.author.toLower().contains(query))
                continue;
        }
        m_articles.append(a);
    }

    // Rebuild index map
    for (int i = 0; i < m_articles.size(); ++i)
        m_indexMap.insert(m_articles[i].id, i);

    endResetModel();
    emit countChanged();
}

// ── ArticleFilterProxy ─────────────────────────────────────────────────

ArticleFilterProxy::ArticleFilterProxy(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setSortRole(ArticleListModel::PublishedRole);
    sort(0, Qt::DescendingOrder);
}

void ArticleFilterProxy::setSearchQuery(const QString &query)
{
    m_searchQuery = query;
    beginFilterChange();
    invalidateFilter();
    endFilterChange();
}

bool ArticleFilterProxy::filterAcceptsRow(int sourceRow,
                                            const QModelIndex &sourceParent) const
{
    if (m_searchQuery.isEmpty())
        return true;

    const QModelIndex idx = sourceModel()->index(sourceRow, 0, sourceParent);
    const QString title = sourceModel()->data(idx, ArticleListModel::TitleRole).toString();
    const QString summary = sourceModel()->data(idx, ArticleListModel::SummaryRole).toString();
    const QString author = sourceModel()->data(idx, ArticleListModel::AuthorRole).toString();

    const QString q = m_searchQuery.toLower();
    return title.toLower().contains(q) ||
           summary.toLower().contains(q) ||
           author.toLower().contains(q);
}

} // namespace drift
