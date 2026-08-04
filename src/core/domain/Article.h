#pragma once

#include <QString>
#include <QDateTime>
#include <QJsonObject>

namespace drift {

/// Represents a single article/item from a feed.
struct Article {
    QString id;              // UUID, generated on creation
    QString feedId;          // Parent feed id
    QString guid;            // Original GUID from the feed (may be empty)
    QString title;
    QString author;
    QString link;            // URL to the original article
    QString summary;         // Short plain-text excerpt
    QString contentHash;     // SHA-1 of the article HTML content
    bool contentCached = false;

    QDateTime published;
    QDateTime updated;
    QDateTime fetched;

    bool isRead = false;
    bool isStarred = false;

    QString thumbPath;       // Local path to thumbnail
    bool hasEnclosure = false;

    QJsonObject toJson() const;
    static Article fromJson(const QJsonObject &obj);
};

} // namespace drift
