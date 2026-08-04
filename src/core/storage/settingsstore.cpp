#include "core/storage/SettingsStore.h"
#include "core/storage/JsonStore.h"
#include "platform/PlatformInfo.h"

namespace drift {

SettingsStore::SettingsStore(QObject *parent)
    : QObject(parent)
{
    m_saveTimer.setSingleShot(true);
    m_saveTimer.setInterval(500);
    connect(&m_saveTimer, &QTimer::timeout, this, &SettingsStore::save);
}

void SettingsStore::load()
{
    const QJsonObject obj = JsonStore::loadJson(filePath());
    if (obj.isEmpty()) {
        m_loaded = true;
        return;
    }

    m_refreshIntervalMinutes = obj.value(QStringLiteral("refreshIntervalMinutes")).toInt(60);
    m_refreshOnStartup = obj.value(QStringLiteral("refreshOnStartup")).toBool(true);
    m_cacheLimitMb = obj.value(QStringLiteral("cacheLimitMb")).toInt(200);
    m_retentionDays = obj.value(QStringLiteral("retentionDays")).toInt(90);
    m_maxArticlesPerFeed = obj.value(QStringLiteral("maxArticlesPerFeed")).toInt(1000);
    m_readerFontSize = obj.value(QStringLiteral("readerFontSize")).toInt(16);
    m_theme = obj.value(QStringLiteral("theme")).toString(QStringLiteral("auto"));
    m_windowWidth = obj.value(QStringLiteral("windowWidth")).toInt(1280);
    m_windowHeight = obj.value(QStringLiteral("windowHeight")).toInt(800);

    m_loaded = true;
    emit loaded();
}

void SettingsStore::save()
{
    QJsonObject obj;
    obj[QStringLiteral("version")] = 1;
    obj[QStringLiteral("refreshIntervalMinutes")] = m_refreshIntervalMinutes;
    obj[QStringLiteral("refreshOnStartup")] = m_refreshOnStartup;
    obj[QStringLiteral("cacheLimitMb")] = m_cacheLimitMb;
    obj[QStringLiteral("retentionDays")] = m_retentionDays;
    obj[QStringLiteral("maxArticlesPerFeed")] = m_maxArticlesPerFeed;
    obj[QStringLiteral("readerFontSize")] = m_readerFontSize;
    obj[QStringLiteral("theme")] = m_theme;
    obj[QStringLiteral("windowWidth")] = m_windowWidth;
    obj[QStringLiteral("windowHeight")] = m_windowHeight;

    JsonStore::saveJson(obj, filePath());
    m_dirty = false;
}

QString SettingsStore::filePath() const
{
    return PlatformInfo::dataRoot() + QStringLiteral("/settings.json");
}

// ── Setters with auto-save ─────────────────────────────────────────────

void SettingsStore::setRefreshIntervalMinutes(int v) {
    if (m_refreshIntervalMinutes != v) { m_refreshIntervalMinutes = v; m_dirty = true; m_saveTimer.start(); emit settingsChanged(); }
}
void SettingsStore::setRefreshOnStartup(bool v) {
    if (m_refreshOnStartup != v) { m_refreshOnStartup = v; m_dirty = true; m_saveTimer.start(); emit settingsChanged(); }
}
void SettingsStore::setCacheLimitMb(int v) {
    if (m_cacheLimitMb != v) { m_cacheLimitMb = v; m_dirty = true; m_saveTimer.start(); emit settingsChanged(); }
}
void SettingsStore::setRetentionDays(int v) {
    if (m_retentionDays != v) { m_retentionDays = v; m_dirty = true; m_saveTimer.start(); emit settingsChanged(); }
}
void SettingsStore::setMaxArticlesPerFeed(int v) {
    if (m_maxArticlesPerFeed != v) { m_maxArticlesPerFeed = v; m_dirty = true; m_saveTimer.start(); emit settingsChanged(); }
}
void SettingsStore::setReaderFontSize(int v) {
    if (m_readerFontSize != v) { m_readerFontSize = v; m_dirty = true; m_saveTimer.start(); emit settingsChanged(); }
}
void SettingsStore::setTheme(const QString &v) {
    if (m_theme != v) { m_theme = v; m_dirty = true; m_saveTimer.start(); emit settingsChanged(); }
}
void SettingsStore::setWindowWidth(int v) {
    if (m_windowWidth != v) { m_windowWidth = v; m_dirty = true; m_saveTimer.start(); emit settingsChanged(); }
}
void SettingsStore::setWindowHeight(int v) {
    if (m_windowHeight != v) { m_windowHeight = v; m_dirty = true; m_saveTimer.start(); emit settingsChanged(); }
}

} // namespace drift
