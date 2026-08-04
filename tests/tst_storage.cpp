#include <QtTest>
#include "core/storage/JsonStore.h"
#include "core/storage/SettingsStore.h"
#include "core/storage/SubscriptionStore.h"
#include "core/domain/Article.h"
#include "core/domain/Feed.h"
#include "core/domain/Category.h"
#include "platform/PlatformInfo.h"
#include <QTemporaryDir>
#include <QUuid>

using namespace drift;

class TestStorage : public QObject {
    Q_OBJECT

private slots:
    void testJsonRoundTrip()
    {
        // Test Article JSON round-trip
        Article original;
        original.id = "test-id-1";
        original.feedId = "feed-1";
        original.guid = "guid-123";
        original.title = "Test Article";
        original.author = "Test Author";
        original.link = "https://example.com/article";
        original.summary = "This is a test.";
        original.isRead = false;
        original.isStarred = true;

        QJsonObject json = original.toJson();
        Article restored = Article::fromJson(json);

        QCOMPARE(restored.id, original.id);
        QCOMPARE(restored.title, original.title);
        QCOMPARE(restored.author, original.author);
        QCOMPARE(restored.link, original.link);
        QCOMPARE(restored.isStarred, original.isStarred);
    }

    void testFeedJsonRoundTrip()
    {
        Feed original;
        original.id = "feed-id-1";
        original.title = "Test Feed";
        original.url = "https://example.com/feed.xml";
        original.siteUrl = "https://example.com";
        original.etag = "W/\"abc123\"";
        original.refreshIntervalMinutes = 30;

        QJsonObject json = original.toJson();
        Feed restored = Feed::fromJson(json);

        QCOMPARE(restored.id, original.id);
        QCOMPARE(restored.title, original.title);
        QCOMPARE(restored.url, original.url);
        QCOMPARE(restored.etag, original.etag);
        QCOMPARE(restored.refreshIntervalMinutes, 30);
    }

    void testCategoryJsonRoundTrip()
    {
        Category original;
        original.id = "cat-1";
        original.name = "Technology";
        original.expanded = true;

        QJsonObject json = original.toJson();
        Category restored = Category::fromJson(json);

        QCOMPARE(restored.id, original.id);
        QCOMPARE(restored.name, original.name);
        QCOMPARE(restored.expanded, original.expanded);
    }
};

QTEST_MAIN(TestStorage)
#include "tst_storage.moc"
