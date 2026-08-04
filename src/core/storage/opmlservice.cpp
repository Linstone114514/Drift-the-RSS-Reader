#include "core/storage/OpmlService.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QStack>

namespace drift {

bool OpmlService::exportOpml(const QString &filePath,
                              const QList<Feed> &feeds,
                              const QList<Category> &categories)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();

    writer.writeStartElement(QStringLiteral("opml"));
    writer.writeAttribute(QStringLiteral("version"), QStringLiteral("2.0"));

    writer.writeStartElement(QStringLiteral("head"));
    writer.writeTextElement(QStringLiteral("title"), QStringLiteral("Drift Subscriptions"));
    writer.writeEndElement(); // head

    writer.writeStartElement(QStringLiteral("body"));

    // Export feeds grouped by category
    QHash<QString, Category> catMap;
    for (const auto &cat : categories)
        catMap.insert(cat.id, cat);

    // Uncategorized feeds first
    for (const auto &feed : feeds) {
        if (!feed.categoryId.isEmpty())
            continue;

        writer.writeStartElement(QStringLiteral("outline"));
        writer.writeAttribute(QStringLiteral("type"), QStringLiteral("rss"));
        writer.writeAttribute(QStringLiteral("text"), feed.title);
        writer.writeAttribute(QStringLiteral("title"), feed.title);
        writer.writeAttribute(QStringLiteral("xmlUrl"), feed.url);
        writer.writeAttribute(QStringLiteral("htmlUrl"), feed.siteUrl);
        writer.writeEndElement();
    }

    // Categorized feeds
    for (const auto &cat : categories) {
        writer.writeStartElement(QStringLiteral("outline"));
        writer.writeAttribute(QStringLiteral("text"), cat.name);
        writer.writeAttribute(QStringLiteral("title"), cat.name);

        for (const auto &feed : feeds) {
            if (feed.categoryId != cat.id)
                continue;

            writer.writeStartElement(QStringLiteral("outline"));
            writer.writeAttribute(QStringLiteral("type"), QStringLiteral("rss"));
            writer.writeAttribute(QStringLiteral("text"), feed.title);
            writer.writeAttribute(QStringLiteral("title"), feed.title);
            writer.writeAttribute(QStringLiteral("xmlUrl"), feed.url);
            writer.writeAttribute(QStringLiteral("htmlUrl"), feed.siteUrl);
            writer.writeEndElement();
        }

        writer.writeEndElement(); // category outline
    }

    writer.writeEndElement(); // body
    writer.writeEndElement(); // opml
    writer.writeEndDocument();

    return true;
}

QList<OpmlOutline> OpmlService::importOpml(const QString &filePath)
{
    QList<OpmlOutline> outlines;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return outlines;

    QXmlStreamReader reader(&file);
    QStack<QString> categoryStack;

    while (!reader.atEnd() && !reader.hasError()) {
        reader.readNext();
        if (reader.isStartElement()) {
            if (reader.name() == QLatin1String("outline")) {
                const QString type = reader.attributes().value("xmlUrl").isEmpty()
                    ? QString() : QStringLiteral("rss");

                if (type == QLatin1String("rss")) {
                    // This is a feed outline
                    OpmlOutline outline;
                    outline.title = reader.attributes().value("title").toString();
                    if (outline.title.isEmpty())
                        outline.title = reader.attributes().value("text").toString();
                    outline.xmlUrl = reader.attributes().value("xmlUrl").toString();
                    outline.htmlUrl = reader.attributes().value("htmlUrl").toString();

                    // Determine category from stack
                    if (!categoryStack.isEmpty()) {
                        QStringList cats;
                        for (int i = 0; i < categoryStack.size(); ++i)
                            cats.append(categoryStack.at(i));
                        outline.categoryName = cats.join(QStringLiteral(" / "));
                    }

                    if (!outline.xmlUrl.isEmpty())
                        outlines.append(outline);
                } else {
                    // This is a category container
                    QString catName = reader.attributes().value("title").toString();
                    if (catName.isEmpty())
                        catName = reader.attributes().value("text").toString();
                    categoryStack.push(catName);
                }
            }
        } else if (reader.isEndElement()) {
            if (reader.name() == QLatin1String("outline") && !categoryStack.isEmpty()) {
                categoryStack.pop();
            }
        }
    }

    return outlines;
}

} // namespace drift
