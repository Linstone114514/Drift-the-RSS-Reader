#include "core/feed/FeedFetcher.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QTimer>

namespace drift {

FeedFetcher::FeedFetcher(QObject *parent)
    : QObject(parent)
    , m_nam(new QNetworkAccessManager(this))
{
}

void FeedFetcher::fetch(const Feed &feed)
{
    refreshAll({feed});
}

void FeedFetcher::refreshAll(const QList<Feed> &feeds)
{
    // Append non-paused feeds to the queue
    for (const auto &f : feeds) {
        if (!f.paused)
            m_queue.append(f);
    }
    dequeue();
}

void FeedFetcher::cancel()
{
    m_queue.clear();
    m_pending = 0;
    m_done = 0;
}

void FeedFetcher::dequeue()
{
    while (m_pending < m_maxConcurrent && !m_queue.isEmpty()) {
        const Feed feed = m_queue.takeFirst();
        ++m_pending;

        QNetworkRequest request(QUrl(feed.url));
        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                             QNetworkRequest::NoLessSafeRedirectPolicy);
        request.setTransferTimeout(30000); // 30 second timeout

        // ETag / Last-Modified conditional request
        if (!feed.etag.isEmpty())
            request.setRawHeader("If-None-Match", feed.etag.toUtf8());
        if (!feed.lastModified.isEmpty())
            request.setRawHeader("If-Modified-Since", feed.lastModified.toUtf8());

        QNetworkReply *reply = m_nam->get(request);
        reply->setProperty("feedId", feed.id);
        connect(reply, &QNetworkReply::finished, this, &FeedFetcher::onReplyFinished);
    }
}

void FeedFetcher::onReplyFinished()
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply)
        return;

    reply->deleteLater();
    --m_pending;

    const QString feedId = reply->property("feedId").toString();
    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (reply->error() == QNetworkReply::NoError) {
        if (status == 304) {
            // Not modified
            emit feedNotModified(feedId);
        } else {
            const QString etag = reply->rawHeader("ETag");
            const QString lastMod = reply->rawHeader("Last-Modified");
            emit feedDownloaded(feedId, reply->readAll(), etag, lastMod);
        }
    } else {
        emit feedFailed(feedId, reply->errorString());
    }

    ++m_done;
    const int total = m_done + m_queue.size() + m_pending;
    emit progressChanged(m_done, total);

    // Continue dequeuing
    dequeue();

    if (m_pending == 0 && m_queue.isEmpty()) {
        emit allDone();
        m_done = 0;
    }
}

} // namespace drift
