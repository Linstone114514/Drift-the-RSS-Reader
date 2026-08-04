#include <QtTest>
#include "core/feed/FeedParser.h"

using namespace drift;

static const char *rss20Sample =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<rss version=\"2.0\">\n"
    "  <channel>\n"
    "    <title>Test Blog</title>\n"
    "    <link>https://example.com</link>\n"
    "    <description>A test blog</description>\n"
    "    <item>\n"
    "      <title>Hello World</title>\n"
    "      <link>https://example.com/hello</link>\n"
    "      <guid isPermaLink=\"false\">post-1</guid>\n"
    "      <pubDate>Mon, 04 Aug 2026 10:00:00 GMT</pubDate>\n"
    "      <description>First post content</description>\n"
    "      <author>test@example.com (Test Author)</author>\n"
    "    </item>\n"
    "  </channel>\n"
    "</rss>";

static const char *atom10Sample =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<feed xmlns=\"http://www.w3.org/2005/Atom\">\n"
    "  <title>Atom Blog</title>\n"
    "  <subtitle>Thoughts and ramblings</subtitle>\n"
    "  <link href=\"https://example.com\"/>\n"
    "  <entry>\n"
    "    <title>Atom Post</title>\n"
    "    <id>tag:example.com,2026:post-1</id>\n"
    "    <link href=\"https://example.com/atom-post\"/>\n"
    "    <published>2026-08-04T10:00:00Z</published>\n"
    "    <summary>Summary text</summary>\n"
    "    <content type=\"html\">&lt;p&gt;Full content&lt;/p&gt;</content>\n"
    "    <author><name>Atom Author</name></author>\n"
    "  </entry>\n"
    "</feed>";

static const char *emptyRssSample =
    "<?xml version=\"1.0\"?>\n"
    "<rss version=\"2.0\">\n"
    "  <channel>\n"
    "    <title>Empty Blog</title>\n"
    "    <link>https://empty.example.com</link>\n"
    "  </channel>\n"
    "</rss>";

class TestFeedParser : public QObject {
    Q_OBJECT

private slots:
    void testParseRss20()
    {
        const QByteArray xml(rss20Sample);
        const auto result = FeedParser::parse(xml, QUrl("https://example.com/feed.xml"));
        QVERIFY(result.error.isEmpty());
        QCOMPARE(result.meta.title, QString("Test Blog"));
        QCOMPARE(result.meta.siteUrl, QString("https://example.com"));
        QCOMPARE(result.items.size(), 1);
        QCOMPARE(result.items[0].title, QString("Hello World"));
        QCOMPARE(result.items[0].guid, QString("post-1"));
    }

    void testParseAtom10()
    {
        const QByteArray xml(atom10Sample);
        const auto result = FeedParser::parse(xml, QUrl("https://example.com/atom.xml"));
        QVERIFY(result.error.isEmpty());
        QCOMPARE(result.meta.title, QString("Atom Blog"));
        QCOMPARE(result.items.size(), 1);
        QCOMPARE(result.items[0].title, QString("Atom Post"));
        QCOMPARE(result.items[0].author, QString("Atom Author"));
    }

    void testMalformedXml()
    {
        const QByteArray xml("This is not XML at all");
        const auto result = FeedParser::parse(xml, QUrl("https://example.com/bad.xml"));
        QVERIFY(!result.error.isEmpty());
        QVERIFY(result.items.isEmpty());
    }

    void testEmptyFeed()
    {
        const QByteArray xml(emptyRssSample);
        const auto result = FeedParser::parse(xml, QUrl("https://empty.example.com/feed.xml"));
        QVERIFY(result.error.isEmpty());
        QCOMPARE(result.items.size(), 0);
    }
};

QTEST_MAIN(TestFeedParser)
#include "tst_feedparser.moc"
