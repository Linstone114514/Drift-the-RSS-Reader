#include "core/domain/Category.h"

namespace drift {

QJsonObject Category::toJson() const
{
    QJsonObject obj;
    obj[QStringLiteral("id")] = id;
    obj[QStringLiteral("name")] = name;
    obj[QStringLiteral("expanded")] = expanded;
    return obj;
}

Category Category::fromJson(const QJsonObject &obj)
{
    Category c;
    c.id = obj.value(QStringLiteral("id")).toString();
    c.name = obj.value(QStringLiteral("name")).toString();
    c.expanded = obj.value(QStringLiteral("expanded")).toBool(true);
    return c;
}

} // namespace drift
