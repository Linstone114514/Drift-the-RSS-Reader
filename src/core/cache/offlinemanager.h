#pragma once

#include <QObject>

namespace drift {

/// Monitors online/offline status for the app.
/// Uses QNetworkInformation when available, falls back to heuristic.
class OfflineManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isOffline READ isOffline NOTIFY offlineChanged)

public:
    explicit OfflineManager(QObject *parent = nullptr);

    bool isOffline() const { return m_offline; }

signals:
    void offlineChanged(bool offline);

private:
    void detectNetworkState();

    bool m_offline = false;
    bool m_initialized = false;
};

} // namespace drift
