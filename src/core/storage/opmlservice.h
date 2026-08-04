#pragma once

#include <QString>
#include <QList>
#include "core/domain/Feed.h"
#include "core/domain/Category.h"

namespace drift {

/// Represents one outline entry parsed from an OPML file.
struct OpmlOutline {
    QString title;
    QString xmlUrl;         // Feed URL
    QString htmlUrl;        // Site URL
    QString categoryName;   // Derived from nesting
};

/// OPML 2.0 import/export service.
class OpmlService {
public:
    OpmlService() = delete;

    /// Export feeds and categories to an OPML file.
    static bool exportOpml(const QString &filePath,
                           const QList<Feed> &feeds,
                           const QList<Category> &categories);

    /// Import outlines from an OPML file (does not add feeds — caller does that).
    static QList<OpmlOutline> importOpml(const QString &filePath);
};

} // namespace drift
