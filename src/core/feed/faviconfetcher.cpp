#include "core/feed/FaviconFetcher.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QImage>
#include <QDir>
#include "platform/PlatformInfo.h"

namespace drift {

FaviconFetcher::FaviconFetcher(QObject *parent)
    : QObject(parent)
    , m_nam(new QNetworkAccessManager(this))
{
}

void FaviconFetcher::fetchIcon(const QString &feedId, const QString &siteUrl,
                                const QString &feedUrl)
{
    Q_UNUSED(feedUrl)

    // Try /favicon.ico on the site's root
    QUrl baseUrl(siteUrl.isEmpty() ? feedUrl : siteUrl);
    QUrl iconUrl = baseUrl;
    iconUrl.setPath(QStringLiteral("/favicon.ico"));

    QNetworkRequest request(iconUrl);
    request.setTransferTimeout(10000);

    QNetworkReply *reply = m_nam->get(request);
    reply->setProperty("feedId", feedId);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        const QString fid = reply->property("feedId").toString();

        if (reply->error() == QNetworkReply::NoError) {
            QImage img;
            if (img.loadFromData(reply->readAll())) {
                img = img.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);

                const QString dirPath = PlatformInfo::dataRoot() + QStringLiteral("/cache/favicons");
                QDir().mkpath(dirPath);
                const QString filePath = dirPath + QLatin1Char('/') + fid + QStringLiteral(".png");

                if (img.save(filePath, "PNG")) {
                    emit iconReady(fid, filePath);
                }
            }
        }
        m_pending.remove(fid);
    });

    m_pending.insert(feedId, siteUrl);
}

} // namespace drift
