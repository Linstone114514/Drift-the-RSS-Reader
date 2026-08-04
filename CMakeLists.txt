cmake_minimum_required(VERSION 3.25)
project(Drift VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Required Qt modules ──────────────────────────────────────────────────
find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core Gui Qml Quick QuickControls2 QuickLayouts Network Concurrent
)

# ── Qt 6 standard setup (AUTOMOC, AUTORCC, AUTOUIC) ──────────────────────
qt_standard_project_setup()

# Qt CMake policies
qt_policy(SET QTP0001 NEW)   # Use :/qt/qml/ resource prefix
qt_policy(SET QTP0004 OLD)   # Suppress qmldir requirement for subdirectories

# ── Optional: Qt WebView for in-app browser ──────────────────────────────
find_package(Qt6 QUIET COMPONENTS WebView)
if(Qt6WebView_FOUND)
    message(STATUS "Qt WebView found — in-app browser enabled")
    add_compile_definitions(DRIFT_HAS_WEBVIEW=1)
else()
    message(STATUS "Qt WebView NOT found — external browser fallback will be used")
    add_compile_definitions(DRIFT_HAS_WEBVIEW=0)
endif()

# ── Platform detection ───────────────────────────────────────────────────
if(ANDROID)
    message(STATUS "Target platform: Android")
elseif(WIN32)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "ARM64|aarch64")
        message(STATUS "Target platform: Windows ARM64")
    else()
        message(STATUS "Target platform: Windows x86_64")
    endif()
elseif(APPLE)
    if(IOS)
        message(STATUS "Target platform: iOS")
    else()
        message(STATUS "Target platform: macOS")
    endif()
endif()

# ── Subdirectories ───────────────────────────────────────────────────────
add_subdirectory(src/core)
add_subdirectory(src/app)

# ── Tests ────────────────────────────────────────────────────────────────
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
