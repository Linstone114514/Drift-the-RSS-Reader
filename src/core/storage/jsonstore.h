#pragma once

#include <QJsonObject>
#include <QJsonArray>
#include <QString>

namespace drift {

/// Base utility for atomic JSON file I/O via QSaveFile.
class JsonStore {
public:
    JsonStore() = delete;

    /// Atomically save a JSON object to disk.
    static bool saveJson(const QJsonObject &obj, const QString &path);

    /// Save a JSON array to disk.
    static bool saveJsonArray(const QJsonArray &arr, const QString &path);

    /// Load a JSON object from disk. Returns empty object on failure.
    static QJsonObject loadJson(const QString &path);

    /// Load a JSON array from disk. Returns empty array on failure.
    static QJsonArray loadJsonArray(const QString &path);

    /// Ensure a directory exists (creates all parent dirs).
    static bool ensureDir(const QString &path);

    /// Check if a file exists.
    static bool fileExists(const QString &path);

    /// Remove a file. Returns true on success or if file doesn't exist.
    static bool removeFile(const QString &path);
};

} // namespace drift
