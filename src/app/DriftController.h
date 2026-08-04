#pragma once

#include <QObject>
#include <QUrl>
#include "core/feed/FeedFetcher.h"
#include "core/feed/FaviconFetcher.h"
#include "core/storage/SettingsStore.h"
#include "core/storage/SubscriptionStore.h"
#include "core/storage/ArticleStore.h"
#include "core/cache/ContentCache.h"
#include "core/cache/HtmlConverter.h"
#include "core/cache/OfflineManager.h"
#include "core/models/FeedTreeModel.h"
#include "core/models/ArticleListModel.h"

namespace drift {

/// Single QML-facing facade that owns all services and models.
/// Registered as a QML singleton under the "Drift" module.
class DriftController : public QObject {
    Q_OBJECT

    // ── Properties exposed to QML ──────────────────────────────────
    Q_PROPERTY(FeedTreeModel *feedTreeModel READ feedTreeModel CONSTANT)
    Q_PROPERTY(ArticleListModel *articleListModel READ articleListModel CONSTANT)
    Q_PROPERTY(SettingsStore *settings READ settings CONSTANT)
    Q_PROPERTY(bool isRefreshing READ isRefreshing NOTIFY isRefreshingChanged)
    Q_PROPERTY(int refreshProgressDone READ refreshProgressDone NOTIFY refreshProgressChanged)
    Q_PROPERTY(int refreshProgressTotal READ refreshProgressTotal NOTIFY refreshProgressChanged)
    Q_PROPERTY(bool isOffline READ isOffline NOTIFY offlineChanged)

public:
    explicit DriftController(QObject *parent = nullptr);

    void initialize();

    FeedTreeModel *feedTreeModel() const { return m_feedTreeModel; }
    ArticleListModel *articleListModel() const { return m_articleListModel; }
    SettingsStore *settings() const { return m_settingsStore; }
    bool isRefreshing() const;
    int refreshProgressDone() const { return m_refreshDone; }
    int refreshProgressTotal() const { return m_refreshTotal; }
    bool isOffline() const;

signals:
    void isRefreshingChanged();
    void refreshProgressChanged();
    void offlineChanged();
    void notification(const QString &title, const QString &message);

    // ── Slots callable from QML ────────────────────────────────────

public slots:
    // Feed management
    void addFeed(const QString &url);
    void removeFeed(const QString &feedId);
    void refreshAll();
    void refreshFeed(const QString &feedId);

    // Article actions
    void markRead(const QStringList &articleIds, bool read = true);
    void markAllRead();
    void markFeedRead(const QString &feedId);
    void toggleStar(const QString &articleId);

    // View switching
    void setView(int view, const QString &feedId = {});

    // Reader
    QString readerContent(const QString &articleId);
    void openOriginal(const QString &url);

    // OPML
    QString exportOpml(const QString &filePath);
    QString importOpml(const QString &filePath);

    // Window state
    void saveWindowGeometry(int width, int height);

private slots:
    void onFeedDownloaded(const QString &feedId, const QByteArray &body,
                          const QString &etag, const QString &lastModified);
    void onFeedNotModified(const QString &feedId);
    void onFeedFailed(const QString &feedId, const QString &error);
    void onRefreshProgress(int done, int total);
    void onAllDone();
    void onFaviconReady(const QString &feedId, const QString &localPath);

private:
    // Services
    SettingsStore *m_settingsStore = nullptr;
    SubscriptionStore *m_subscriptionStore = nullptr;
    ArticleStore *m_articleStore = nullptr;
    FeedFetcher *m_feedFetcher = nullptr;
    FaviconFetcher *m_faviconFetcher = nullptr;
    ContentCache *m_contentCache = nullptr;
    HtmlConverter *m_htmlConverter = nullptr;
    OfflineManager *m_offlineManager = nullptr;

    // Models
    FeedTreeModel *m_feedTreeModel = nullptr;
    ArticleListModel *m_articleListModel = nullptr;

    int m_refreshDone = 0;
    int m_refreshTotal = 0;
};

} // namespace drift
