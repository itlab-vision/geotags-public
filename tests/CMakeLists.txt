# Find GTest package
find_package(GTest REQUIRED)

# Get all common test sources, including tests in module subdirectories.
file(GLOB_RECURSE TEST_SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "test_*.cpp"
)
list(FILTER TEST_SOURCES EXCLUDE REGEX "/(fb_structs|not_fb_structs)/")

# Add sources from the selected data structure implementation.
if(USE_FLATBUFFERS_STRUCTURES)
    file(GLOB_RECURSE TEST_VARIANT_SOURCES
        RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "*/fb_structs/test_*.cpp"
    )
else()
    file(GLOB_RECURSE TEST_VARIANT_SOURCES
        RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "*/not_fb_structs/test_*.cpp"
    )
endif()
list(APPEND TEST_SOURCES ${TEST_VARIANT_SOURCES})

# create test executable (all files at once)
add_executable(run_tests ${TEST_SOURCES})

target_compile_definitions(run_tests PRIVATE
    TEST_DATA_DIR="/${CMAKE_CURRENT_SOURCE_DIR}/tests_data/"
)

# link with Google Test and project library
target_link_libraries(run_tests
    sanitizers
    ${GEOTAGS_LIBRARY}
)

# set properties
set_target_properties(run_tests PROPERTIES
    OUTPUT_NAME "run_tests"
    PROJECT_LABEL "run_tests"
    RUNTIME_OUTPUT_DIRECTORY "../")
