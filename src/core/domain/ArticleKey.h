#pragma once

#include <QString>
#include <QCryptographicHash>

namespace drift {

/// Canonical deduplication key for articles.
/// Priority: SHA-1(guid) > SHA-1(link) > SHA-1(title|published)
inline QString articleKey(const QString &guid, const QString &link,
                           const QString &title, const QString &published)
{
    auto sha1 = [](const QString &input) -> QString {
        return QString::fromLatin1(
            QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Sha1).toHex());
    };

    if (!guid.isEmpty())
        return sha1(guid);
    if (!link.isEmpty())
        return sha1(link);
    return sha1(title + QLatin1Char('|') + published);
}

} // namespace drift
