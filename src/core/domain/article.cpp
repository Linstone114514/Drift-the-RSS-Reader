#include "core/domain/Article.h"

namespace drift {

QJsonObject Article::toJson() const
{
    QJsonObject obj;
    obj[QStringLiteral("id")] = id;
    obj[QStringLiteral("feedId")] = feedId;
    obj[QStringLiteral("guid")] = guid;
    obj[QStringLiteral("title")] = title;
    obj[QStringLiteral("author")] = author;
    obj[QStringLiteral("link")] = link;
    obj[QStringLiteral("summary")] = summary;
    obj[QStringLiteral("contentHash")] = contentHash;
    obj[QStringLiteral("contentCached")] = contentCached;
    obj[QStringLiteral("published")] = published.toString(Qt::ISODate);
    obj[QStringLiteral("updated")] = updated.toString(Qt::ISODate);
    obj[QStringLiteral("fetched")] = fetched.toString(Qt::ISODate);
    obj[QStringLiteral("isRead")] = isRead;
    obj[QStringLiteral("isStarred")] = isStarred;
    obj[QStringLiteral("thumbPath")] = thumbPath;
    obj[QStringLiteral("hasEnclosure")] = hasEnclosure;
    return obj;
}

Article Article::fromJson(const QJsonObject &obj)
{
    Article a;
    a.id = obj.value(QStringLiteral("id")).toString();
    a.feedId = obj.value(QStringLiteral("feedId")).toString();
    a.guid = obj.value(QStringLiteral("guid")).toString();
    a.title = obj.value(QStringLiteral("title")).toString();
    a.author = obj.value(QStringLiteral("author")).toString();
    a.link = obj.value(QStringLiteral("link")).toString();
    a.summary = obj.value(QStringLiteral("summary")).toString();
    a.contentHash = obj.value(QStringLiteral("contentHash")).toString();
    a.contentCached = obj.value(QStringLiteral("contentCached")).toBool(false);
    a.published = QDateTime::fromString(
        obj.value(QStringLiteral("published")).toString(), Qt::ISODate);
    a.updated = QDateTime::fromString(
        obj.value(QStringLiteral("updated")).toString(), Qt::ISODate);
    a.fetched = QDateTime::fromString(
        obj.value(QStringLiteral("fetched")).toString(), Qt::ISODate);
    a.isRead = obj.value(QStringLiteral("isRead")).toBool(false);
    a.isStarred = obj.value(QStringLiteral("isStarred")).toBool(false);
    a.thumbPath = obj.value(QStringLiteral("thumbPath")).toString();
    a.hasEnclosure = obj.value(QStringLiteral("hasEnclosure")).toBool(false);
    return a;
}

} // namespace drift
