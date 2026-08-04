# ── Test targets ─────────────────────────────────────────────────────────

find_package(Qt6 REQUIRED COMPONENTS Test)

set(CMAKE_AUTOMOC ON)
enable_testing()

# Helper to add a test
function(drift_add_test name)
    add_executable(${name}
        ${name}.cpp
    )
    target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(${name} PRIVATE DriftCore Qt6::Core Qt6::Test)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

# Storage tests
drift_add_test(tst_storage)

# Feed parser tests
drift_add_test(tst_feedparser)

# OPML tests
drift_add_test(tst_opml)

# HTML converter tests
drift_add_test(tst_html)
