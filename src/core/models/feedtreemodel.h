#pragma once

#include <QAbstractItemModel>
#include <QList>
#include "core/domain/Feed.h"
#include "core/domain/Category.h"

namespace drift {

class SubscriptionStore;
class ArticleStore;

/// Hierarchical model for the feed sidebar tree.
class FeedTreeModel : public QAbstractItemModel {
    Q_OBJECT
    Q_PROPERTY(int viewAllIndex READ viewAllIndex CONSTANT)
    Q_PROPERTY(int viewUnreadIndex READ viewUnreadIndex CONSTANT)
    Q_PROPERTY(int viewStarredIndex READ viewStarredIndex CONSTANT)
    Q_PROPERTY(int viewTodayIndex READ viewTodayIndex CONSTANT)

public:
    enum NodeType { VirtualNode, CategoryNode, FeedNode };
    Q_ENUM(NodeType)

    enum Roles {
        NodeTypeRole = Qt::UserRole + 1,
        TitleRole,
        FeedIdRole,
        FeedUrlRole,
        CategoryIdRole,
        IconPathRole,
        UnreadCountRole,
        IsRefreshingRole,
        IsExpandedRole,
        VirtualViewRole  // int: 0=All, 1=Unread, 2=Starred, 3=Today
    };
    Q_ENUM(Roles)

    explicit FeedTreeModel(SubscriptionStore *subStore, ArticleStore *artStore,
                           QObject *parent = nullptr);

    int viewAllIndex() const { return 0; }
    int viewUnreadIndex() const { return 1; }
    int viewStarredIndex() const { return 2; }
    int viewTodayIndex() const { return 3; }

    /// Find the model index for a given feed ID.
    QModelIndex indexForFeed(const QString &feedId) const;

    // QAbstractItemModel interface
    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void onFeedAdded(const drift::Feed &feed);
    void onFeedUpdated(const drift::Feed &feed);
    void onFeedRemoved(const QString &feedId);
    void onArticlesChanged();
    void onArticlesAdded(const QString &feedId, const QStringList &newIds);

private:
    void rebuild();

    struct TreeNode;
    using NodePtr = std::shared_ptr<TreeNode>;

    struct TreeNode {
        NodeType type;
        int row = 0;
        // VirtualNode
        int virtualView = -1; // 0=All, 1=Unread, 2=Starred, 3=Today
        // CategoryNode
        Category category;
        // FeedNode
        Feed feed;
        // Tree structure
        NodePtr parent;
        QList<NodePtr> children;
    };

    NodePtr m_root;
    SubscriptionStore *m_subStore;
    ArticleStore *m_artStore;
};

} // namespace drift
