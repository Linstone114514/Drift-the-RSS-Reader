#pragma once

#include <QString>
#include <QDateTime>
#include <QJsonObject>

namespace drift {

/// Represents a single RSS/Atom feed subscription.
struct Feed {
    QString id;                     // UUID, generated on creation
    QString title;                  // Feed channel title
    QString url;                    // XML feed URL
    QString siteUrl;                // Website URL
    QString description;            // Feed description
    QString categoryId;             // Parent category id (empty = uncategorized)
    QString iconPath;               // Local path to favicon (cache/favicons/<id>.png)

    // HTTP caching headers
    QString etag;
    QString lastModified;

    QDateTime lastFetched;
    QString lastError;
    int refreshIntervalMinutes = 60;
    bool paused = false;

    QJsonObject toJson() const;
    static Feed fromJson(const QJsonObject &obj);

    bool operator==(const Feed &other) const { return id == other.id; }
};

} // namespace drift
