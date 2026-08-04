#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QSortFilterProxyModel>
#include "core/domain/Article.h"

namespace drift {

class ArticleStore;
class SubscriptionStore;

/// Flat list model for the article list, backed by ArticleStore.
class ArticleListModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QString searchQuery READ searchQuery NOTIFY searchQueryChanged)

public:
    enum View { AllView, UnreadView, StarredView, TodayView, FeedView };

    enum Roles {
        ArticleIdRole = Qt::UserRole + 1,
        FeedIdRole,
        TitleRole,
        SummaryRole,
        AuthorRole,
        PublishedRole,
        PublishedRelativeRole,
        FeedNameRole,
        FeedIconRole,
        IsReadRole,
        IsStarredRole,
        ThumbPathRole,
        HasThumbRole,
        ContentCachedRole,
        LinkRole
    };
    Q_ENUM(Roles)

    explicit ArticleListModel(ArticleStore *artStore, SubscriptionStore *subStore,
                              QObject *parent = nullptr);

    int count() const { return m_articles.size(); }

    /// Switch the active view.
    Q_INVOKABLE void setView(int view, const QString &feedId = {});

    /// Search filter.
    Q_INVOKABLE void setSearchQuery(const QString &query);
    QString searchQuery() const;

    /// Show only unread.
    Q_INVOKABLE void setShowUnreadOnly(bool unreadOnly);

    /// Get article at index.
    Q_INVOKABLE QVariantMap get(int row) const;

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void countChanged();
    void searchQueryChanged();

private slots:
    void onArticlesAdded(const QString &feedId, const QStringList &newIds);
    void onArticleFlagsChanged(const QStringList &ids);
    void reload();

private:
    void applyFilters();

    ArticleStore *m_artStore;
    SubscriptionStore *m_subStore;

    View m_view = AllView;
    QString m_feedId;
    QString m_searchQuery;
    bool m_showUnreadOnly = false;

    QList<Article> m_allArticles;   // Unfiltered source
    QList<Article> m_articles;      // Filtered + sorted
    QHash<QString, int> m_indexMap; // articleId -> index in m_articles
};

/// Proxy model that provides additional filtering/sorting on top of ArticleListModel.
class ArticleFilterProxy : public QSortFilterProxyModel {
    Q_OBJECT
    Q_PROPERTY(QString searchQuery WRITE setSearchQuery)

public:
    explicit ArticleFilterProxy(QObject *parent = nullptr);

    void setSearchQuery(const QString &query);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QString m_searchQuery;
};

} // namespace drift
