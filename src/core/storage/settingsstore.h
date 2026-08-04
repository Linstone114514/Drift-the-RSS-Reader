#pragma once

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QTimer>

namespace drift {

/// Application settings backed by a JSON file.
/// Exposed to QML as a singleton via QML_SINGLETON.
class SettingsStore : public QObject {
    Q_OBJECT
    Q_PROPERTY(int refreshIntervalMinutes READ refreshIntervalMinutes WRITE setRefreshIntervalMinutes NOTIFY settingsChanged)
    Q_PROPERTY(bool refreshOnStartup READ refreshOnStartup WRITE setRefreshOnStartup NOTIFY settingsChanged)
    Q_PROPERTY(int cacheLimitMb READ cacheLimitMb WRITE setCacheLimitMb NOTIFY settingsChanged)
    Q_PROPERTY(int retentionDays READ retentionDays WRITE setRetentionDays NOTIFY settingsChanged)
    Q_PROPERTY(int maxArticlesPerFeed READ maxArticlesPerFeed WRITE setMaxArticlesPerFeed NOTIFY settingsChanged)
    Q_PROPERTY(int readerFontSize READ readerFontSize WRITE setReaderFontSize NOTIFY settingsChanged)
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY settingsChanged)
    Q_PROPERTY(int windowWidth READ windowWidth WRITE setWindowWidth NOTIFY settingsChanged)
    Q_PROPERTY(int windowHeight READ windowHeight WRITE setWindowHeight NOTIFY settingsChanged)

public:
    explicit SettingsStore(QObject *parent = nullptr);

    void load();
    void save();

    // Accessors
    int refreshIntervalMinutes() const { return m_refreshIntervalMinutes; }
    void setRefreshIntervalMinutes(int v);

    bool refreshOnStartup() const { return m_refreshOnStartup; }
    void setRefreshOnStartup(bool v);

    int cacheLimitMb() const { return m_cacheLimitMb; }
    void setCacheLimitMb(int v);

    int retentionDays() const { return m_retentionDays; }
    void setRetentionDays(int v);

    int maxArticlesPerFeed() const { return m_maxArticlesPerFeed; }
    void setMaxArticlesPerFeed(int v);

    int readerFontSize() const { return m_readerFontSize; }
    void setReaderFontSize(int v);

    QString theme() const { return m_theme; }
    void setTheme(const QString &v);

    int windowWidth() const { return m_windowWidth; }
    void setWindowWidth(int v);

    int windowHeight() const { return m_windowHeight; }
    void setWindowHeight(int v);

signals:
    void settingsChanged();
    void loaded();

private:
    QString filePath() const;

    int m_refreshIntervalMinutes = 60;
    bool m_refreshOnStartup = true;
    int m_cacheLimitMb = 200;
    int m_retentionDays = 90;
    int m_maxArticlesPerFeed = 1000;
    int m_readerFontSize = 16;
    QString m_theme = QStringLiteral("auto");
    int m_windowWidth = 1280;
    int m_windowHeight = 800;

    QTimer m_saveTimer;
    bool m_dirty = false;
    bool m_loaded = false;
};

} // namespace drift
