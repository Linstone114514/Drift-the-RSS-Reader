#pragma once

#include <QObject>
#include <QList>
#include <QHash>
#include <QTimer>
#include "core/domain/Feed.h"
#include "core/domain/Category.h"

namespace drift {

/// Manages feed subscriptions and categories, backed by subscriptions.json.
class SubscriptionStore : public QObject {
    Q_OBJECT

public:
    explicit SubscriptionStore(QObject *parent = nullptr);

    void load();
    void save();

    /// Write pending debounced changes to disk immediately.
    void flush();

    // Categories
    void addCategory(const Category &cat);
    void removeCategory(const QString &categoryId);
    void updateCategory(const Category &cat);
    QList<Category> categories() const { return m_categories.values(); }
    Category categoryById(const QString &id) const { return m_categories.value(id); }

    // Feeds
    void addFeed(const Feed &feed);
    void updateFeed(const Feed &feed);
    void removeFeed(const QString &feedId);
    QList<Feed> feeds() const { return m_feeds; }
    QList<Feed> feedsForCategory(const QString &categoryId) const;
    Feed feedById(const QString &feedId) const;
    Feed feedByUrl(const QString &url) const;

signals:
    void feedAdded(const drift::Feed &feed);
    void feedUpdated(const drift::Feed &feed);
    void feedRemoved(const QString &feedId);
    void categoriesChanged();
    void storeLoaded();

private:
    QString filePath() const;
    void scheduleSave();

    QList<Feed> m_feeds;
    QHash<QString, Category> m_categories;
    QTimer m_saveTimer;
};

} // namespace drift
