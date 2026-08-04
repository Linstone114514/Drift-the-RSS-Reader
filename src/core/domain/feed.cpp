#include "core/domain/Feed.h"

namespace drift {

QJsonObject Feed::toJson() const
{
    QJsonObject obj;
    obj[QStringLiteral("id")] = id;
    obj[QStringLiteral("title")] = title;
    obj[QStringLiteral("url")] = url;
    obj[QStringLiteral("siteUrl")] = siteUrl;
    obj[QStringLiteral("description")] = description;
    obj[QStringLiteral("categoryId")] = categoryId;
    obj[QStringLiteral("iconPath")] = iconPath;
    obj[QStringLiteral("etag")] = etag;
    obj[QStringLiteral("lastModified")] = lastModified;
    obj[QStringLiteral("lastFetched")] = lastFetched.toString(Qt::ISODate);
    obj[QStringLiteral("lastError")] = lastError;
    obj[QStringLiteral("refreshIntervalMinutes")] = refreshIntervalMinutes;
    obj[QStringLiteral("paused")] = paused;
    return obj;
}

Feed Feed::fromJson(const QJsonObject &obj)
{
    Feed f;
    f.id = obj.value(QStringLiteral("id")).toString();
    f.title = obj.value(QStringLiteral("title")).toString();
    f.url = obj.value(QStringLiteral("url")).toString();
    f.siteUrl = obj.value(QStringLiteral("siteUrl")).toString();
    f.description = obj.value(QStringLiteral("description")).toString();
    f.categoryId = obj.value(QStringLiteral("categoryId")).toString();
    f.iconPath = obj.value(QStringLiteral("iconPath")).toString();
    f.etag = obj.value(QStringLiteral("etag")).toString();
    f.lastModified = obj.value(QStringLiteral("lastModified")).toString();
    f.lastFetched = QDateTime::fromString(
        obj.value(QStringLiteral("lastFetched")).toString(), Qt::ISODate);
    f.lastError = obj.value(QStringLiteral("lastError")).toString();
    f.refreshIntervalMinutes = obj.value(QStringLiteral("refreshIntervalMinutes")).toInt(60);
    f.paused = obj.value(QStringLiteral("paused")).toBool(false);
    return f;
}

} // namespace drift
