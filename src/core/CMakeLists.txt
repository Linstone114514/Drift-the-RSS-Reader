# ── DriftCore static library ─────────────────────────────────────────────
# All parsing, storage, networking, and caching logic lives here.
# QML only sees models + the DriftController facade.

add_library(DriftCore STATIC
    # Domain
    domain/feed.cpp
    domain/article.cpp
    domain/category.cpp
    domain/articlekey.cpp

    # Feed layer
    feed/feedfetcher.cpp
    feed/feedparser.cpp
    feed/faviconfetcher.cpp

    # Storage layer
    storage/jsonstore.cpp
    storage/settingsstore.cpp
    storage/subscriptionstore.cpp
    storage/articlestore.cpp
    storage/opmlservice.cpp

    # Cache layer
    cache/contentcache.cpp
    cache/htmlconverter.cpp
    cache/offlinemanager.cpp

    # Models
    models/feedtreemodel.cpp
    models/articlelistmodel.cpp
)

target_include_directories(DriftCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(DriftCore
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Network
        Qt6::Concurrent
)

# Platform detection for DriftCore
if(WIN32)
    target_sources(DriftCore PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../platform/platforminfo_win.cpp)
elseif(ANDROID)
    target_sources(DriftCore PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../platform/platforminfo_android.cpp)
elseif(APPLE)
    target_sources(DriftCore PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../platform/platforminfo_apple.cpp)
else()
    target_sources(DriftCore PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../platform/platforminfo_stub.cpp)
endif()
