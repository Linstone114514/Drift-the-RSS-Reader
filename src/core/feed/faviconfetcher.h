#pragma once

#include <QObject>
#include <QHash>
#include <QNetworkAccessManager>

namespace drift {

/// Discovers and downloads favicons for feeds.
class FaviconFetcher : public QObject {
    Q_OBJECT

public:
    explicit FaviconFetcher(QObject *parent = nullptr);

    /// Start fetching the favicon for a feed.
    void fetchIcon(const QString &feedId, const QString &siteUrl, const QString &feedUrl);

signals:
    void iconReady(const QString &feedId, const QString &localPath);

private:
    QNetworkAccessManager *m_nam = nullptr;
    QHash<QString, QString> m_pending; // feedId -> siteUrl
};

} // namespace drift
