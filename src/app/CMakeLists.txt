# ── Drift executable ─────────────────────────────────────────────────────

qt_add_executable(drift
    main.cpp
    DriftController.h
    DriftController.cpp
    MANUAL_FINALIZATION
)

# QML module — all QML files live here, URI "Drift"
qt_add_qml_module(drift
    URI Drift
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/MainWindow.qml
        qml/Sidebar/FeedTreeView.qml
        qml/Sidebar/FeedNodeDelegate.qml
        qml/Sidebar/ViewChips.qml
        qml/ArticleList/ArticleListView.qml
        qml/ArticleList/ArticleDelegate.qml
        qml/ArticleList/ListHeader.qml
        qml/ArticleList/SearchField.qml
        qml/Reader/ReaderView.qml
        qml/Reader/ReaderMarkdown.qml
        qml/Reader/WebViewPane.qml
        qml/Dialogs/AddFeedDialog.qml
        qml/Dialogs/OpmlDialog.qml
        qml/Dialogs/FeedSettingsDialog.qml
        qml/Dialogs/AboutDialog.qml
        qml/components/UnreadBadge.qml
        qml/components/StarButton.qml
        qml/components/EmptyState.qml
)

# Optional WebView support
if(Qt6WebView_FOUND)
    target_link_libraries(drift PRIVATE Qt6::WebView)
endif()

target_link_libraries(drift
    PRIVATE
        DriftCore
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::QuickLayouts
)

# Platform-specific executable properties
# Note: WIN32_EXECUTABLE is FALSE on MinGW to avoid Qt6EntryPoint / __imp___argc
# linker error with MinGW-w64 >= 13.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set_target_properties(drift PROPERTIES WIN32_EXECUTABLE FALSE)
    target_link_options(drift PRIVATE -mwindows)
else()
    set_target_properties(drift PROPERTIES
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE
    )
endif()

# Android package source
if(ANDROID)
    set_target_properties(drift PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
    )
endif()

# Required for static builds (iOS) and finalizes the app target
qt_finalize_executable(drift)
