#pragma once

#include <QString>
#include <QUrl>
#include <QObject>

namespace drift {

/// Converts article HTML to Markdown for the offline reader view.
class HtmlConverter : public QObject {
    Q_OBJECT

public:
    explicit HtmlConverter(QObject *parent = nullptr);

    /// Convert HTML to Markdown. If cacheImages is true, downloads and
    /// rewrites image URLs to local file:// paths via ContentCache.
    QString toMarkdown(const QString &html, const QUrl &baseUrl,
                       const QString &articleId = {});

    /// Extract plain text summary from HTML.
    QString toPlainText(const QString &html);

private:
    // ContentCache reference is owned by DriftController; HtmlConverter
    // receives it as needed via method parameters. Image caching callbacks
    // are driven from DriftController which wires ContentCache in.
};

} // namespace drift
