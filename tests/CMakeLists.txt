# Find GTest package
find_package(GTest REQUIRED)

# get all test source files including main
if(USE_FLATBUFFERS_STRUCTURES)
    file(GLOB TEST_SOURCES 
        RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
        "test_*.cpp"
        "fb_structs/test_*.cpp"
    )
else()
    file(GLOB TEST_SOURCES 
        RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
        "test_*.cpp"
        "not_fb_structs/test_*.cpp"
    )
endif()

# create test executable (all files at once)
add_executable(run_tests ${TEST_SOURCES})

target_compile_definitions(run_tests PRIVATE
    TEST_DATA_DIR="/${CMAKE_CURRENT_SOURCE_DIR}/tests_data/"
)

# link with Google Test and project library
target_link_libraries(run_tests 
    GTest::GTest
    ${GEOTAGS_LIBRARY}
)

# set properties
set_target_properties(run_tests PROPERTIES
    OUTPUT_NAME "run_tests"
    PROJECT_LABEL "run_tests"
    RUNTIME_OUTPUT_DIRECTORY "../")
