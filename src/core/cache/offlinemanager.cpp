#include "core/cache/OfflineManager.h"
#include <QNetworkInformation>
#include <QTimer>

namespace drift {

OfflineManager::OfflineManager(QObject *parent)
    : QObject(parent)
{
    detectNetworkState();
}

void OfflineManager::detectNetworkState()
{
    // Try QNetworkInformation (Qt 6.3+)
    if (QNetworkInformation::loadDefaultBackend()) {
        auto *info = QNetworkInformation::instance();
        m_offline = info->reachability() == QNetworkInformation::Reachability::Disconnected;

        connect(info, &QNetworkInformation::reachabilityChanged, this,
                [this](QNetworkInformation::Reachability reach) {
            bool offline = (reach == QNetworkInformation::Reachability::Disconnected);
            if (m_offline != offline) {
                m_offline = offline;
                emit offlineChanged(m_offline);
            }
        });

        m_initialized = true;
    } else {
        // Fallback: assume online. The offline banner only shows if
        // we're sure we're offline.
        m_offline = false;
        m_initialized = true;
    }
}

} // namespace drift
