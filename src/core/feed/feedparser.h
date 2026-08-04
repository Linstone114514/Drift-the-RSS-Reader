#pragma once

#include <QString>
#include <QList>
#include <QUrl>
#include <QDateTime>

namespace drift {

/// Result of parsing a single feed item.
struct ParsedItem {
    QString guid;
    QString title;
    QString author;
    QString link;
    QString summary;        // First paragraph / description
    QString contentHtml;    // Full HTML content (content:encoded or atom:content)
    QDateTime published;
    QDateTime updated;
    QString thumbUrl;       // media:thumbnail or enclosure
    bool hasEnclosure = false;
};

/// Metadata extracted from the feed channel element.
struct FeedMeta {
    QString title;
    QString description;
    QString siteUrl;
    QString iconUrl;        // <icon>, <logo>, or link[rel=icon]
};

/// Result of a feed parse operation.
struct FeedParseResult {
    QList<ParsedItem> items;
    FeedMeta meta;
    QString error;          // Empty on success
};

/// Pure static parser for RSS and Atom feeds.
class FeedParser {
public:
    FeedParser() = delete;

    /// Parse raw XML and return structured result.
    /// @param xml      Raw feed XML bytes.
    /// @param sourceUrl The URL the feed was fetched from (for relative URL resolution).
    static FeedParseResult parse(const QByteArray &xml, const QUrl &sourceUrl);

private:
    static FeedParseResult parseRss(const QByteArray &xml, const QUrl &sourceUrl);
    static FeedParseResult parseAtom(const QByteArray &xml, const QUrl &sourceUrl);
    static QDateTime parseRfc2822(const QString &dateStr);
    static QDateTime parseRfc3339(const QString &dateStr);
};

} // namespace drift
