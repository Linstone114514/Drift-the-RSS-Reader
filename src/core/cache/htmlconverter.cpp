#include "core/cache/HtmlConverter.h"
#include <QTextDocument>
#include <QTextBlock>
#include <QTextFragment>
#include <QRegularExpression>

namespace drift {

HtmlConverter::HtmlConverter(QObject *parent)
    : QObject(parent)
{
}

QString HtmlConverter::toMarkdown(const QString &html, const QUrl &baseUrl,
                                   const QString &articleId)
{
    Q_UNUSED(articleId)

    // Use QTextDocument for initial HTML parsing and sanitization.
    // Qt's HTML importer ignores scripts and styles, providing safe content.
    QTextDocument doc;
    doc.setDefaultStyleSheet(QStringLiteral("a { color: inherit; }"));
    doc.setHtml(html);

    // Convert to Markdown using Qt's built-in converter (5.14+)
    QString markdown = doc.toMarkdown();

    // Clean up: remove excessive blank lines
    QRegularExpression multiNewline(QStringLiteral("\n{3,}"));
    markdown.replace(multiNewline, QStringLiteral("\n\n"));

    return markdown.trimmed();
}

QString HtmlConverter::toPlainText(const QString &html)
{
    QTextDocument doc;
    doc.setHtml(html);
    QString text = doc.toPlainText();

    // Truncate to a reasonable summary length
    if (text.length() > 500) {
        text = text.left(500);
        // Try to break at word boundary
        int lastSpace = text.lastIndexOf(QLatin1Char(' '));
        if (lastSpace > 300)
            text = text.left(lastSpace);
        text += QStringLiteral("...");
    }

    return text.trimmed();
}

} // namespace drift
