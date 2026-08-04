#include <QtTest>
#include <QTemporaryDir>
#include "core/storage/OpmlService.h"
#include "core/domain/Feed.h"
#include "core/domain/Category.h"

using namespace drift;

class TestOpml : public QObject {
    Q_OBJECT

private slots:
    void testExportImportRoundTrip()
    {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString filePath = dir.path() + "/test.opml";

        // Create test data
        QList<Feed> feeds;
        Feed f1;
        f1.id = "feed-1";
        f1.title = "Test Blog";
        f1.url = "https://example.com/feed.xml";
        f1.siteUrl = "https://example.com";
        feeds.append(f1);

        Feed f2;
        f2.id = "feed-2";
        f2.title = "Another Blog";
        f2.url = "https://another.example.com/rss";
        f2.siteUrl = "https://another.example.com";
        feeds.append(f2);

        QList<Category> categories;
        Category cat;
        cat.id = "cat-1";
        cat.name = "Technology";
        categories.append(cat);

        // Export
        bool ok = OpmlService::exportOpml(filePath, feeds, categories);
        QVERIFY(ok);
        QVERIFY(QFileInfo::exists(filePath));

        // Import
        const auto outlines = OpmlService::importOpml(filePath);
        QVERIFY(!outlines.isEmpty());
        QCOMPARE(outlines.size(), 2);

        // Verify feed URLs are preserved
        QStringList urls;
        for (const auto &o : outlines)
            urls.append(o.xmlUrl);
        QVERIFY(urls.contains("https://example.com/feed.xml"));
        QVERIFY(urls.contains("https://another.example.com/rss"));
    }

    void testImportNonexistentFile()
    {
        const auto outlines = OpmlService::importOpml("/nonexistent/path/file.opml");
        QVERIFY(outlines.isEmpty());
    }
};

QTEST_MAIN(TestOpml)
#include "tst_opml.moc"
