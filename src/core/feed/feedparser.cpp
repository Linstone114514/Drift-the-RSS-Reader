#include "core/feed/FeedParser.h"
#include <QXmlStreamReader>

namespace drift {

FeedParseResult FeedParser::parse(const QByteArray &xml, const QUrl &sourceUrl)
{
    QXmlStreamReader reader(xml);
    reader.readNextStartElement();

    const QString rootName = reader.name().toString();
    const QString ns = reader.namespaceUri().toString();

    // Detect format from root element
    if (rootName == QLatin1String("rss")) {
        return parseRss(xml, sourceUrl);
    } else if (rootName == QLatin1String("feed") &&
               (ns.contains(QStringLiteral("Atom")) || ns.contains(QStringLiteral("atom")))) {
        return parseAtom(xml, sourceUrl);
    } else if (rootName == QLatin1String("RDF") &&
               ns.contains(QStringLiteral("rdf"))) {
        // RSS 1.0 / RDF — delegate to RSS parser with RDF awareness
        return parseRss(xml, sourceUrl);
    }

    // Unknown format — try RSS first, then Atom
    FeedParseResult result = parseRss(xml, sourceUrl);
    if (!result.error.isEmpty()) {
        result = parseAtom(xml, sourceUrl);
    }
    return result;
}

FeedParseResult FeedParser::parseRss(const QByteArray &xml, const QUrl &sourceUrl)
{
    Q_UNUSED(sourceUrl)
    FeedParseResult result;

    QXmlStreamReader reader(xml);

    while (!reader.atEnd() && !reader.hasError()) {
        reader.readNext();
        if (reader.isStartElement()) {
            if (reader.name() == QLatin1String("channel")) {
                // Parse channel metadata
                while (!(reader.isEndElement() && reader.name() == QLatin1String("channel"))) {
                    reader.readNext();
                    if (reader.isStartElement()) {
                        if (reader.name() == QLatin1String("title")) {
                            result.meta.title = reader.readElementText();
                        } else if (reader.name() == QLatin1String("description")) {
                            result.meta.description = reader.readElementText();
                        } else if (reader.name() == QLatin1String("link")) {
                            result.meta.siteUrl = reader.readElementText();
                        } else if (reader.name() == QLatin1String("item")) {
                            ParsedItem item;
                            // Parse item
                            while (!(reader.isEndElement() && reader.name() == QLatin1String("item"))) {
                                reader.readNext();
                                if (reader.isStartElement()) {
                                    const QStringView name = reader.name();
                                    if (name == QLatin1String("title")) {
                                        item.title = reader.readElementText();
                                    } else if (name == QLatin1String("link")) {
                                        item.link = reader.readElementText();
                                    } else if (name == QLatin1String("description")) {
                                        item.summary = reader.readElementText();
                                    } else if (name == QLatin1String("content:encoded") ||
                                               reader.qualifiedName() == QLatin1String("content:encoded")) {
                                        item.contentHtml = reader.readElementText();
                                    } else if (name == QLatin1String("author") ||
                                               name == QLatin1String("dc:creator")) {
                                        item.author = reader.readElementText();
                                    } else if (name == QLatin1String("guid")) {
                                        item.guid = reader.readElementText();
                                    } else if (name == QLatin1String("pubDate")) {
                                        item.published = parseRfc2822(reader.readElementText());
                                    } else if (name == QLatin1String("enclosure")) {
                                        item.hasEnclosure = true;
                                        item.thumbUrl = reader.attributes().value("url").toString();
                                        reader.skipCurrentElement();
                                    } else if (name == QLatin1String("media:thumbnail") ||
                                               reader.qualifiedName() == QLatin1String("media:thumbnail")) {
                                        item.thumbUrl = reader.attributes().value("url").toString();
                                        reader.skipCurrentElement();
                                    } else {
                                        reader.skipCurrentElement();
                                    }
                                }
                            }
                            if (!item.title.isEmpty() || !item.link.isEmpty()) {
                                result.items.append(std::move(item));
                            }
                        }
                    }
                }
            }
        }
    }

    if (reader.hasError()) {
        result.error = reader.errorString();
    }

    return result;
}

FeedParseResult FeedParser::parseAtom(const QByteArray &xml, const QUrl &sourceUrl)
{
    Q_UNUSED(sourceUrl)
    FeedParseResult result;

    QXmlStreamReader reader(xml);

    while (!reader.atEnd() && !reader.hasError()) {
        reader.readNext();
        if (reader.isStartElement()) {
            if (reader.name() == QLatin1String("feed")) {
                while (!(reader.isEndElement() && reader.name() == QLatin1String("feed"))) {
                    reader.readNext();
                    if (reader.isStartElement()) {
                        if (reader.name() == QLatin1String("title")) {
                            result.meta.title = reader.readElementText();
                        } else if (reader.name() == QLatin1String("subtitle")) {
                            result.meta.description = reader.readElementText();
                        } else if (reader.name() == QLatin1String("icon")) {
                            result.meta.iconUrl = reader.readElementText();
                        } else if (reader.name() == QLatin1String("entry")) {
                            ParsedItem item;
                            while (!(reader.isEndElement() && reader.name() == QLatin1String("entry"))) {
                                reader.readNext();
                                if (reader.isStartElement()) {
                                    const QStringView name = reader.name();
                                    if (name == QLatin1String("title")) {
                                        item.title = reader.readElementText();
                                    } else if (name == QLatin1String("id")) {
                                        item.guid = reader.readElementText();
                                    } else if (name == QLatin1String("published")) {
                                        item.published = parseRfc3339(reader.readElementText());
                                    } else if (name == QLatin1String("updated")) {
                                        item.updated = parseRfc3339(reader.readElementText());
                                    } else if (name == QLatin1String("summary")) {
                                        item.summary = reader.readElementText();
                                    } else if (name == QLatin1String("content")) {
                                        item.contentHtml = reader.readElementText();
                                    } else if (name == QLatin1String("link")) {
                                        const QString rel = reader.attributes().value("rel").toString();
                                        if (rel.isEmpty() || rel == QLatin1String("alternate")) {
                                            item.link = reader.attributes().value("href").toString();
                                        }
                                        reader.skipCurrentElement();
                                    } else if (name == QLatin1String("author")) {
                                        // Parse author/name
                                        while (!(reader.isEndElement() && reader.name() == QLatin1String("author"))) {
                                            reader.readNext();
                                            if (reader.isStartElement() && reader.name() == QLatin1String("name")) {
                                                item.author = reader.readElementText();
                                            }
                                        }
                                    } else {
                                        reader.skipCurrentElement();
                                    }
                                }
                            }
                            if (!item.title.isEmpty() || !item.link.isEmpty()) {
                                result.items.append(std::move(item));
                            }
                        }
                    }
                }
            }
        }
    }

    if (reader.hasError()) {
        result.error = reader.errorString();
    }

    return result;
}

QDateTime FeedParser::parseRfc2822(const QString &dateStr)
{
    // RFC 2822 / RFC 822 date format used in RSS 2.0
    // e.g. "Mon, 04 Aug 2026 10:00:00 GMT"
    // Qt's RFC2822Date format handles this
    QDateTime dt = QDateTime::fromString(dateStr.trimmed(), Qt::RFC2822Date);
    if (!dt.isValid()) {
        // Try ISO as fallback
        dt = QDateTime::fromString(dateStr.trimmed(), Qt::ISODate);
    }
    return dt;
}

QDateTime FeedParser::parseRfc3339(const QString &dateStr)
{
    // RFC 3339 / ISO 8601 used in Atom
    // e.g. "2026-08-04T10:00:00Z" or "2026-08-04T10:00:00+02:00"
    QDateTime dt = QDateTime::fromString(dateStr.trimmed(), Qt::ISODate);
    if (!dt.isValid()) {
        dt = QDateTime::fromString(dateStr.trimmed(), Qt::RFC2822Date);
    }
    return dt;
}

} // namespace drift
