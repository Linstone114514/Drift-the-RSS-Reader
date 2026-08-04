#include "platform/PlatformInfo.h"
#include <QStandardPaths>
#include <QDir>

namespace drift {

QString PlatformInfo::dataRoot()
{
    // Android: QStandardPaths::AppDataLocation returns the app's internal files dir.
    const QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(path);
    return path;
}

QString PlatformInfo::platformName()
{
    return QStringLiteral("Android");
}

} // namespace drift
