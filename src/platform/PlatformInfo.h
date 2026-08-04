#pragma once

#include <QString>

namespace drift {

/// Platform information utilities.
/// Implementation is platform-specific (selected in core/CMakeLists.txt).
struct PlatformInfo {
    /// Returns the data storage root path (QStandardPaths::AppDataLocation + "/Drift").
    static QString dataRoot();

    /// Returns a human-readable platform description for diagnostics.
    static QString platformName();
};

} // namespace drift
