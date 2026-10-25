#pragma once

#include <QObject>
#include <QHash>
#include <QList>
#include <QTimer>
#include "core/domain/Article.h"

namespace drift {

/// Manages all articles in-memory with per-feed JSON shard persistence.
class ArticleStore : public QObject {
    Q_OBJECT

public:
    explicit ArticleStore(QObject *parent = nullptr);

    void load();
    void loadFeed(const QString &feedId);

    /// Insert or update articles for a feed. Returns IDs of NEW articles.
    QList<QString> upsertFeedArticles(const QString &feedId, const QList<Article> &incoming);

    /// Single-article operations
    void setRead(const QStringList &articleIds, bool read);
    void setStarred(const QStringList &articleIds, bool starred);
    void clearContentCached(const QString &articleId);
    void markFeedRead(const QString &feedId);
    void markAllRead();

    /// Queries
    QList<Article> articlesForFeed(const QString &feedId) const;
    QList<Article> starredArticles() const;
    QList<Article> unreadArticles() const;
    QList<Article> recentArticles(int days = 1) const;
    QList<Article> searchArticles(const QString &query) const;
    Article articleById(const QString &id) const;
    int unreadCount(const QString &feedId) const;
    int totalUnreadCount() const;

    /// Retention
    void purgeOld(int maxDays, int maxPerFeed);

signals:
    void articlesAdded(const QString &feedId, const QStringList &newIds);
    void articleFlagsChanged(const QStringList &ids);
    void storeLoaded();

private:
    QString feedShardPath(const QString &feedId) const;
    QString starsPath() const;
    QString articleKey(const Article &a) const;
    void saveFeedShard(const QString &feedId);
    void saveStars();
    void loadStars();

    // feedId -> list of articles
    QHash<QString, QList<Article>> m_articles;
    // dedupe key -> article id
    QHash<QString, QString> m_dedupKeys;

    // Starred index: feedId -> set of articleIds
    QHash<QString, QSet<QString>> m_stars;
    QTimer m_starSaveTimer;
};

} // namespace drift
