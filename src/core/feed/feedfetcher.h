#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QList>
#include "core/domain/Feed.h"

namespace drift {

/// Fetches RSS/Atom feeds via HTTP with ETag/Last-Modified support.
class FeedFetcher : public QObject {
    Q_OBJECT

public:
    explicit FeedFetcher(QObject *parent = nullptr);

    /// Fetch a single feed (one-off).
    void fetch(const Feed &feed);

    /// Refresh all feeds in the list (ordered queue, max 4 concurrent).
    void refreshAll(const QList<Feed> &feeds);

    /// Cancel all in-flight requests.
    void cancel();

    bool isRefreshing() const { return m_pending > 0; }

signals:
    void feedDownloaded(const QString &feedId, const QByteArray &body,
                        const QString &etag, const QString &lastModified);
    void feedNotModified(const QString &feedId);
    void feedFailed(const QString &feedId, const QString &error);
    void progressChanged(int done, int total);
    void allDone();

private:
    void dequeue();
    void onReplyFinished();

    QNetworkAccessManager *m_nam = nullptr;
    QList<Feed> m_queue;
    int m_pending = 0;
    int m_done = 0;
    int m_maxConcurrent = 4;
};

} // namespace drift
