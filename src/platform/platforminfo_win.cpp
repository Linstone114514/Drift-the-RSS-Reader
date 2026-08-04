#include "platform/PlatformInfo.h"
#include <QStandardPaths>
#include <QDir>

namespace drift {

QString PlatformInfo::dataRoot()
{
    // AppDataLocation already includes org name + app name.
    // e.g. %APPDATA%/Drift/Drift — no extra suffix needed.
    const QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(path);
    return path;
}

QString PlatformInfo::platformName()
{
#ifdef Q_PROCESSOR_ARM_64
    return QStringLiteral("Windows ARM64");
#else
    return QStringLiteral("Windows x86_64");
#endif
}

} // namespace drift
