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

# ── Windows packaging: NSIS installer via CPack ──────────────────────────
# "cmake --install" (and "cpack -G NSIS") deploys Qt runtime DLLs next to
# drift.exe with windeployqt, then NSIS turns the staging tree into
# Drift-<version>-win64-setup.exe.
if(WIN32 AND NOT ANDROID)
    install(TARGETS drift RUNTIME DESTINATION .)

    find_program(WINDEPLOYQT_EXECUTABLE
        NAMES windeployqt
        PATHS "$ENV{QT_ROOT_DIR}/bin" "$ENV{CMAKE_PREFIX_PATH}/bin"
        REQUIRED
    )
    install(CODE "
        message(STATUS \"Deploying Qt runtime with windeployqt...\")
        execute_process(
            COMMAND \"${WINDEPLOYQT_EXECUTABLE}\" --release
                    --qmldir \"${CMAKE_CURRENT_SOURCE_DIR}/src/app/qml\"
                    \"\${CMAKE_INSTALL_PREFIX}/drift.exe\"
            COMMAND_ERROR_IS_FATAL ANY
        )
    ")

    set(CPACK_GENERATOR "NSIS")
    set(CPACK_PACKAGE_NAME "Drift")
    set(CPACK_PACKAGE_VENDOR "Drift")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A clean, cross-platform desktop RSS/Atom reader")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "Drift")
    set(CPACK_PACKAGE_FILE_NAME "Drift-${PROJECT_VERSION}-win64-setup")

    set(CPACK_NSIS_PACKAGE_NAME "Drift")
    set(CPACK_NSIS_DISPLAY_NAME "Drift")
    set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/src/app/drift.ico")
    set(CPACK_NSIS_MUI_UNICON "${CMAKE_CURRENT_SOURCE_DIR}/src/app/drift.ico")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "drift.exe")
    set(CPACK_PACKAGE_EXECUTABLES "drift;Drift")
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_MODIFY_PATH OFF)
    set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/Linstone114514/Drift-the-RSS-Reader")

    include(CPack)
endif()
