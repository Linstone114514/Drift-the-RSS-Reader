#include "core/storage/JsonStore.h"
#include <QSaveFile>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>

namespace drift {

bool JsonStore::saveJson(const QJsonObject &obj, const QString &path)
{
    ensureDir(QFileInfo(path).absolutePath());

    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    return file.commit();
}

bool JsonStore::saveJsonArray(const QJsonArray &arr, const QString &path)
{
    ensureDir(QFileInfo(path).absolutePath());

    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    file.write(QJsonDocument(arr).toJson(QJsonDocument::Indented));
    return file.commit();
}

QJsonObject JsonStore::loadJson(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return {};

    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject())
        return {};
    return doc.object();
}

QJsonArray JsonStore::loadJsonArray(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return {};

    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError || !doc.isArray())
        return {};
    return doc.array();
}

bool JsonStore::ensureDir(const QString &path)
{
    return QDir().mkpath(path);
}

bool JsonStore::fileExists(const QString &path)
{
    return QFileInfo::exists(path);
}

bool JsonStore::removeFile(const QString &path)
{
    if (!QFileInfo::exists(path))
        return true;
    return QFile::remove(path);
}

} // namespace drift
