#pragma once

#include <QObject>
#include <QHash>
#include <QString>
#include "core/domain/Article.h"

namespace drift {

/// Manages the offline content cache: HTML files, images, thumbnails, favicons.
class ContentCache : public QObject {
    Q_OBJECT

public:
    explicit ContentCache(QObject *parent = nullptr);

    /// Cache the raw HTML content for an article.
    QString cacheArticleContent(const QString &articleId, const QByteArray &html);

    /// Get the cached HTML file path for an article.
    QString cachedContentPath(const QString &articleId) const;

    /// Check if article content is cached.
    bool hasCached(const QString &articleId) const;

    /// Download and cache an image from a URL. Returns local file:// path.
    QString cacheImage(const QUrl &imageUrl, const QString &articleId);

    /// Remove all cached content for an article.
    void removeArticleCache(const QString &articleId);

    /// LRU eviction: prune cache until total size is under limitMb.
    void pruneToLimit(int limitMb);

    /// Clear entire cache.
    void clear();

    /// Total cache size in bytes.
    qint64 totalSize() const;

signals:
    void imageCached(const QString &articleId, const QString &originalUrl,
                     const QString &localPath);

private:
    QString cacheDir() const;
    QString contentDir() const;
    void loadIndex();
    void saveIndex();
    void touchAccess(const QString &path);

    struct CacheEntry {
        QString path;
        qint64 size = 0;
        QDateTime lastAccessed;
    };
    QHash<QString, CacheEntry> m_index; // path -> entry
};

} // namespace drift
