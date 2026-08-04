#pragma once

#include <QString>
#include <QJsonObject>

namespace drift {

/// Represents a category/grouping of feeds.
struct Category {
    QString id;              // UUID
    QString name;
    bool expanded = true;    // UI expansion state

    QJsonObject toJson() const;
    static Category fromJson(const QJsonObject &obj);

    bool operator==(const Category &other) const { return id == other.id; }
};

} // namespace drift
