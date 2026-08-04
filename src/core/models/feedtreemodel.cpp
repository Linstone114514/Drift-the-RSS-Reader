#include "core/models/FeedTreeModel.h"
#include "core/storage/SubscriptionStore.h"
#include "core/storage/ArticleStore.h"

namespace drift {

FeedTreeModel::FeedTreeModel(SubscriptionStore *subStore, ArticleStore *artStore,
                               QObject *parent)
    : QAbstractItemModel(parent)
    , m_subStore(subStore)
    , m_artStore(artStore)
{
    m_root = std::make_shared<TreeNode>();
    m_root->type = VirtualNode;
    m_root->virtualView = -1;

    // Wire store signals
    connect(m_subStore, &SubscriptionStore::feedAdded, this, &FeedTreeModel::onFeedAdded);
    connect(m_subStore, &SubscriptionStore::feedUpdated, this, &FeedTreeModel::onFeedUpdated);
    connect(m_subStore, &SubscriptionStore::feedRemoved, this, &FeedTreeModel::onFeedRemoved);
    connect(m_artStore, &ArticleStore::articlesAdded, this, &FeedTreeModel::onArticlesAdded);
    connect(m_artStore, &ArticleStore::articleFlagsChanged, this, &FeedTreeModel::onArticlesChanged);

    rebuild();
}

QModelIndex FeedTreeModel::indexForFeed(const QString &feedId) const
{
    for (int i = 0; i < m_root->children.size(); ++i) {
        const auto &node = m_root->children[i];
        if (node->type == FeedNode && node->feed.id == feedId)
            return createIndex(i, 0, node.get());
        // Search within categories
        for (int j = 0; j < node->children.size(); ++j) {
            const auto &child = node->children[j];
            if (child->type == FeedNode && child->feed.id == feedId)
                return createIndex(j, 0, child.get());
        }
    }
    return {};
}

// ── QAbstractItemModel interface ───────────────────────────────────────

QModelIndex FeedTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return {};

    const TreeNode *parentNode = parent.isValid()
        ? static_cast<const TreeNode *>(parent.internalPointer())
        : m_root.get();

    if (row < parentNode->children.size())
        return createIndex(row, column, parentNode->children[row].get());

    return {};
}

QModelIndex FeedTreeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return {};

    const auto *childNode = static_cast<const TreeNode *>(child.internalPointer());
    if (!childNode->parent || childNode->parent == m_root)
        return {};

    const auto *parentNode = childNode->parent.get();
    const auto *grandparent = parentNode->parent ? parentNode->parent.get() : m_root.get();

    int row = -1;
    for (int i = 0; i < grandparent->children.size(); ++i) {
        if (grandparent->children[i].get() == parentNode) {
            row = i;
            break;
        }
    }

    if (row >= 0)
        return createIndex(row, 0, const_cast<TreeNode *>(parentNode));

    return {};
}

int FeedTreeModel::rowCount(const QModelIndex &parent) const
{
    const TreeNode *node = parent.isValid()
        ? static_cast<const TreeNode *>(parent.internalPointer())
        : m_root.get();

    return node->children.size();
}

int FeedTreeModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant FeedTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};

    const auto *node = static_cast<const TreeNode *>(index.internalPointer());

    switch (role) {
    case NodeTypeRole:
        return node->type;
    case TitleRole:
        if (node->type == VirtualNode) {
            switch (node->virtualView) {
            case 0: return tr("All Articles");
            case 1: return tr("Unread");
            case 2: return tr("Starred");
            case 3: return tr("Today");
            }
        } else if (node->type == CategoryNode) {
            return node->category.name;
        } else {
            return node->feed.title;
        }
        break;
    case FeedIdRole:
        return node->type == FeedNode ? node->feed.id : QString();
    case FeedUrlRole:
        return node->type == FeedNode ? node->feed.url : QString();
    case CategoryIdRole:
        return node->type == CategoryNode ? node->category.id : QString();
    case IconPathRole:
        return node->type == FeedNode ? node->feed.iconPath : QString();
    case UnreadCountRole: {
        if (node->type == VirtualNode) {
            switch (node->virtualView) {
            case 1: return m_artStore->totalUnreadCount();
            default: return 0;
            }
        } else if (node->type == FeedNode) {
            return m_artStore->unreadCount(node->feed.id);
        } else if (node->type == CategoryNode) {
            int total = 0;
            for (const auto &child : node->children) {
                if (child->type == FeedNode)
                    total += m_artStore->unreadCount(child->feed.id);
            }
            return total;
        }
        return 0;
    }
    case IsRefreshingRole:
        return false;
    case IsExpandedRole:
        if (node->type == CategoryNode)
            return node->category.expanded;
        return true;
    case VirtualViewRole:
        return node->virtualView;
    default:
        break;
    }

    return {};
}

QHash<int, QByteArray> FeedTreeModel::roleNames() const
{
    return {
        { NodeTypeRole, "nodeType" },
        { TitleRole, "title" },
        { FeedIdRole, "feedId" },
        { FeedUrlRole, "feedUrl" },
        { CategoryIdRole, "categoryId" },
        { IconPathRole, "iconPath" },
        { UnreadCountRole, "unreadCount" },
        { IsRefreshingRole, "isRefreshing" },
        { IsExpandedRole, "isExpanded" },
        { VirtualViewRole, "virtualView" }
    };
}

// ── Private slots ──────────────────────────────────────────────────────

void FeedTreeModel::onFeedAdded(const Feed &feed)
{
    Q_UNUSED(feed)
    rebuild();
}

void FeedTreeModel::onFeedUpdated(const Feed &feed)
{
    // Find and update the feed node
    for (int i = 0; i < m_root->children.size(); ++i) {
        const auto &node = m_root->children[i];
        if (node->type == FeedNode && node->feed.id == feed.id) {
            node->feed = feed;
            emit dataChanged(createIndex(i, 0, node.get()),
                             createIndex(i, 0, node.get()));
            return;
        }
        for (int j = 0; j < node->children.size(); ++j) {
            const auto &child = node->children[j];
            if (child->type == FeedNode && child->feed.id == feed.id) {
                child->feed = feed;
                QModelIndex parentIdx = createIndex(i, 0, node.get());
                emit dataChanged(createIndex(j, 0, child.get()),
                                 createIndex(j, 0, child.get()));
                return;
            }
        }
    }
}

void FeedTreeModel::onFeedRemoved(const QString &feedId)
{
    Q_UNUSED(feedId)
    rebuild();
}

void FeedTreeModel::onArticlesChanged()
{
    // Unread counts changed — emit dataChanged for all nodes
    if (!m_root->children.isEmpty())
        emit dataChanged(index(0, 0), index(m_root->children.size() - 1, 0),
                         {UnreadCountRole});
}

void FeedTreeModel::onArticlesAdded(const QString &feedId, const QStringList &newIds)
{
    Q_UNUSED(feedId)
    Q_UNUSED(newIds)
    onArticlesChanged();
}

void FeedTreeModel::rebuild()
{
    beginResetModel();

    m_root->children.clear();

    // Virtual nodes: All, Unread, Starred, Today
    const QStringList virtualTitles = {
        tr("All Articles"), tr("Unread"), tr("Starred"), tr("Today")
    };
    for (int i = 0; i < virtualTitles.size(); ++i) {
        auto node = std::make_shared<TreeNode>();
        node->type = VirtualNode;
        node->virtualView = i;
        node->parent = m_root;
        node->row = i;
        m_root->children.append(node);
    }

    const auto cats = m_subStore->categories();
    const auto feeds = m_subStore->feeds();

    // Feeds without a category go directly under root
    for (const auto &feed : feeds) {
        if (!feed.categoryId.isEmpty())
            continue;
        auto node = std::make_shared<TreeNode>();
        node->type = FeedNode;
        node->feed = feed;
        node->parent = m_root;
        node->row = m_root->children.size();
        m_root->children.append(node);
    }

    // Categorized feeds
    for (const auto &cat : cats) {
        auto catNode = std::make_shared<TreeNode>();
        catNode->type = CategoryNode;
        catNode->category = cat;
        catNode->parent = m_root;
        catNode->row = m_root->children.size();

        for (const auto &feed : feeds) {
            if (feed.categoryId != cat.id)
                continue;
            auto feedNode = std::make_shared<TreeNode>();
            feedNode->type = FeedNode;
            feedNode->feed = feed;
            feedNode->parent = catNode;
            feedNode->row = catNode->children.size();
            catNode->children.append(feedNode);
        }

        m_root->children.append(catNode);
    }

    endResetModel();
}

} // namespace drift
