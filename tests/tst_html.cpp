#include <QtTest>
#include "core/cache/HtmlConverter.h"

using namespace drift;

class TestHtmlConverter : public QObject {
    Q_OBJECT

private slots:
    void testToMarkdown()
    {
        HtmlConverter converter;

        const QString html = "<h1>Title</h1><p>Hello <strong>world</strong></p>";
        const QString md = converter.toMarkdown(html, QUrl());

        QVERIFY(md.contains("Title"));
        QVERIFY(md.contains("Hello"));
        QVERIFY(md.contains("world"));
    }

    void testToPlainText()
    {
        HtmlConverter converter;

        const QString html = "<p>This is a <b>test</b> article.</p><script>alert('xss')</script>";
        const QString text = converter.toPlainText(html);

        QVERIFY(text.contains("test"));
        QVERIFY(text.contains("article"));
        // Script content should not appear
        QVERIFY(!text.contains("xss"));
    }

    void testScriptStripping()
    {
        HtmlConverter converter;

        const QString html = "<div><script>evil()</script><p>Safe text</p></div>";
        const QString text = converter.toPlainText(html);

        QVERIFY(text.contains("Safe"));
        QVERIFY(!text.contains("evil"));
    }
};

QTEST_MAIN(TestHtmlConverter)
#include "tst_html.moc"
