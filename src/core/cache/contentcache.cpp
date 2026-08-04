#include "core/cache/ContentCache.h"
#include "core/storage/JsonStore.h"
#include "platform/PlatformInfo.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QSaveFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QDateTime>

namespace drift {

ContentCache::ContentCache(QObject *parent)
    : QObject(parent)
{
    QDir().mkpath(contentDir());
    loadIndex();
}

QString ContentCache::cacheArticleContent(const QString &articleId, const QByteArray &html)
{
    const QString dirPath = contentDir();
    const QString filePath = dirPath + QLatin1Char('/') + articleId + QStringLiteral(".html");

    QSaveFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(html);
        if (file.commit()) {
            CacheEntry entry;
            entry.path = filePath;
            entry.size = html.size();
            entry.lastAccessed = QDateTime::currentDateTime();
            m_index.insert(filePath, entry);
            saveIndex();
            return filePath;
        }
    }
    return {};
}

QString ContentCache::cachedContentPath(const QString &articleId) const
{
    const QString filePath = contentDir() + QLatin1Char('/') + articleId + QStringLiteral(".html");
    if (m_index.contains(filePath))
        return filePath;
    return {};
}

bool ContentCache::hasCached(const QString &articleId) const
{
    const QString filePath = contentDir() + QLatin1Char('/') + articleId + QStringLiteral(".html");
    return m_index.contains(filePath);
}

QString ContentCache::cacheImage(const QUrl &imageUrl, const QString &articleId)
{
    // Synchronous download — called from worker thread or with small images
    QNetworkAccessManager nam;
    QNetworkRequest request(imageUrl);
    request.setTransferTimeout(10000);

    QNetworkReply *reply = nam.get(request);
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        reply->deleteLater();
        return {};
    }

    const QByteArray data = reply->readAll();
    reply->deleteLater();

    if (data.size() > 2 * 1024 * 1024) // 2 MB limit
        return {};

    // Determine extension
    QString ext = QStringLiteral(".jpg");
    const QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
    if (contentType.contains(QStringLiteral("png")))
        ext = QStringLiteral(".png");
    else if (contentType.contains(QStringLiteral("gif")))
        ext = QStringLiteral(".gif");
    else if (contentType.contains(QStringLiteral("webp")))
        ext = QStringLiteral(".webp");
    else if (contentType.contains(QStringLiteral("svg")))
        ext = QStringLiteral(".svg");

    const QString dirPath = contentDir() + QLatin1Char('/') + articleId + QStringLiteral(".images");
    QDir().mkpath(dirPath);

    const int count = QDir(dirPath).entryList(QDir::Files).size();
    const QString filePath = dirPath + QLatin1Char('/') + QString::number(count) + ext;

    QSaveFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(data);
        if (file.commit()) {
            CacheEntry entry;
            entry.path = filePath;
            entry.size = data.size();
            entry.lastAccessed = QDateTime::currentDateTime();
            m_index.insert(filePath, entry);
            saveIndex();
            return QStringLiteral("file:///") + filePath;
        }
    }

    return {};
}

void ContentCache::removeArticleCache(const QString &articleId)
{
    // Remove HTML file
    const QString htmlPath = contentDir() + QLatin1Char('/') + articleId + QStringLiteral(".html");
    m_index.remove(htmlPath);
    QFile::remove(htmlPath);

    // Remove images directory
    const QString imagesDir = contentDir() + QLatin1Char('/') + articleId + QStringLiteral(".images");
    QDir dir(imagesDir);
    for (const auto &entry : dir.entryList(QDir::Files)) {
        const QString path = imagesDir + QLatin1Char('/') + entry;
        m_index.remove(path);
        QFile::remove(path);
    }
    dir.rmdir(imagesDir);

    saveIndex();
}

void ContentCache::pruneToLimit(int limitMb)
{
    const qint64 limitBytes = static_cast<qint64>(limitMb) * 1024 * 1024;
    qint64 currentSize = totalSize();

    if (currentSize <= limitBytes)
        return;

    // Sort entries by lastAccessed (oldest first)
    QList<CacheEntry> entries = m_index.values();
    std::sort(entries.begin(), entries.end(),
              [](const CacheEntry &a, const CacheEntry &b) {
                  return a.lastAccessed < b.lastAccessed;
              });

    for (const auto &entry : entries) {
        if (currentSize <= limitBytes)
            break;

        QFile::remove(entry.path);
        currentSize -= entry.size;
        m_index.remove(entry.path);
    }

    saveIndex();
}

void ContentCache::clear()
{
    for (const auto &entry : m_index)
        QFile::remove(entry.path);
    m_index.clear();
    saveIndex();
}

qint64 ContentCache::totalSize() const
{
    qint64 size = 0;
    for (const auto &entry : m_index)
        size += entry.size;
    return size;
}

// ── Private ────────────────────────────────────────────────────────────

QString ContentCache::cacheDir() const
{
    return PlatformInfo::dataRoot() + QStringLiteral("/cache");
}

QString ContentCache::contentDir() const
{
    return cacheDir() + QStringLiteral("/content");
}

void ContentCache::loadIndex()
{
    const QJsonObject obj = JsonStore::loadJson(cacheDir() + QStringLiteral("/index.json"));
    const QJsonArray arr = obj.value(QStringLiteral("entries")).toArray();
    for (const auto &v : arr) {
        const QJsonObject entryObj = v.toObject();
        CacheEntry entry;
        entry.path = entryObj.value(QStringLiteral("path")).toString();
        entry.size = static_cast<qint64>(entryObj.value(QStringLiteral("size")).toDouble());
        entry.lastAccessed = QDateTime::fromString(
            entryObj.value(QStringLiteral("lastAccessed")).toString(), Qt::ISODate);
        if (QFileInfo::exists(entry.path))
            m_index.insert(entry.path, entry);
    }
}

void ContentCache::saveIndex()
{
    QJsonArray arr;
    for (const auto &entry : m_index) {
        QJsonObject obj;
        obj[QStringLiteral("path")] = entry.path;
        obj[QStringLiteral("size")] = static_cast<double>(entry.size);
        obj[QStringLiteral("lastAccessed")] = entry.lastAccessed.toString(Qt::ISODate);
        arr.append(obj);
    }
    QJsonObject root;
    root[QStringLiteral("entries")] = arr;
    JsonStore::saveJson(root, cacheDir() + QStringLiteral("/index.json"));
}

void ContentCache::touchAccess(const QString &path)
{
    if (m_index.contains(path)) {
        m_index[path].lastAccessed = QDateTime::currentDateTime();
    }
}

} // namespace drift
