#include "platform/PlatformInfo.h"
#include <QStandardPaths>
#include <QDir>

namespace drift {

QString PlatformInfo::dataRoot()
{
    const QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(path);
    return path;
}

QString PlatformInfo::platformName()
{
    return QStringLiteral("Unknown");
}

} // namespace drift
