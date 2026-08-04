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
#ifdef Q_OS_IOS
    return QStringLiteral("iOS");
#else
    return QStringLiteral("macOS");
#endif
}

} // namespace drift
