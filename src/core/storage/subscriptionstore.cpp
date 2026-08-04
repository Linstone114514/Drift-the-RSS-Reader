#include "core/storage/SubscriptionStore.h"
#include "core/storage/JsonStore.h"
#include "platform/PlatformInfo.h"
#include <QJsonArray>
#include <QUuid>

namespace drift {

SubscriptionStore::SubscriptionStore(QObject *parent)
    : QObject(parent)
{
}

void SubscriptionStore::load()
{
    const QJsonObject obj = JsonStore::loadJson(filePath());
    if (obj.isEmpty())
        return;

    // Parse categories
    const QJsonArray catsArr = obj.value(QStringLiteral("categories")).toArray();
    for (const auto &v : catsArr) {
        Category cat = Category::fromJson(v.toObject());
        m_categories.insert(cat.id, cat);
    }

    // Parse feeds
    const QJsonArray feedsArr = obj.value(QStringLiteral("feeds")).toArray();
    for (const auto &v : feedsArr) {
        m_feeds.append(Feed::fromJson(v.toObject()));
    }

    emit storeLoaded();
}

void SubscriptionStore::save()
{
    QJsonObject obj;
    obj[QStringLiteral("version")] = 1;

    QJsonArray catsArr;
    for (const auto &cat : m_categories)
        catsArr.append(cat.toJson());
    obj[QStringLiteral("categories")] = catsArr;

    QJsonArray feedsArr;
    for (const auto &feed : m_feeds)
        feedsArr.append(feed.toJson());
    obj[QStringLiteral("feeds")] = feedsArr;

    JsonStore::saveJson(obj, filePath());
}

QString SubscriptionStore::filePath() const
{
    return PlatformInfo::dataRoot() + QStringLiteral("/subscriptions.json");
}

// ── Categories ─────────────────────────────────────────────────────────

void SubscriptionStore::addCategory(const Category &cat)
{
    m_categories.insert(cat.id, cat);
    save();
    emit categoriesChanged();
}

void SubscriptionStore::removeCategory(const QString &categoryId)
{
    m_categories.remove(categoryId);
    // Move feeds in this category to uncategorized
    for (auto &f : m_feeds) {
        if (f.categoryId == categoryId)
            f.categoryId.clear();
    }
    save();
    emit categoriesChanged();
}

void SubscriptionStore::updateCategory(const Category &cat)
{
    m_categories.insert(cat.id, cat);
    save();
    emit categoriesChanged();
}

// ── Feeds ──────────────────────────────────────────────────────────────

void SubscriptionStore::addFeed(const Feed &feed)
{
    m_feeds.append(feed);
    save();
    emit feedAdded(feed);
}

void SubscriptionStore::updateFeed(const Feed &feed)
{
    for (auto &f : m_feeds) {
        if (f.id == feed.id) {
            f = feed;
            break;
        }
    }
    save();
    emit feedUpdated(feed);
}

void SubscriptionStore::removeFeed(const QString &feedId)
{
    m_feeds.erase(std::remove_if(m_feeds.begin(), m_feeds.end(),
        [&](const Feed &f) { return f.id == feedId; }), m_feeds.end());
    save();
    emit feedRemoved(feedId);
}

QList<Feed> SubscriptionStore::feedsForCategory(const QString &categoryId) const
{
    QList<Feed> result;
    for (const auto &f : m_feeds) {
        if (f.categoryId == categoryId)
            result.append(f);
    }
    return result;
}

Feed SubscriptionStore::feedById(const QString &feedId) const
{
    for (const auto &f : m_feeds) {
        if (f.id == feedId)
            return f;
    }
    return {};
}

Feed SubscriptionStore::feedByUrl(const QString &url) const
{
    for (const auto &f : m_feeds) {
        if (f.url == url)
            return f;
    }
    return {};
}

} // namespace drift
