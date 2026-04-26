cmake_minimum_required(VERSION 3.24)

set(PROJECT_NAME geotags)
project(${PROJECT_NAME})
set(GEOTAGS_LIBRARY ${PROJECT_NAME})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})


include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

# OPTIONS
option(BUILD_TINYEXIF "Build 3rdparty library TinyEXIF with tinyxml2" OFF)
option(BUILD_OPENCV "Build required modules from OpenCV" OFF)
option(BUILD_GOOGLETEST "Build 3rdparty library GoogleTest" OFF)
option(BUILD_TESTS "Build geotags tests" OFF)
option(BUILD_FLATBUFFERS "Build 3rdparty library FlatBuffers" OFF)
option(BUILD_PYBIND11 "Build 3rdparty library pybind11" OFF)
option(BUILD_PYTHON_BINDINGS "Build geotags Python bindings" OFF)
option(USE_FLATBUFFERS_STRUCTURES "Use FlatBuffers internal structures instead of serialization" OFF)

set(LIBRARY_DEPS "")

# DEFINITIONS
if(USE_FLATBUFFERS_STRUCTURES)
    add_compile_definitions(USE_FLATBUFFERS_STRUCTURES)
endif()


# TINYEXIF
if(BUILD_TINYEXIF)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    set(BUILD_SHARED_LIBS ON CACHE BOOL "")
    set(BUILD_TESTING OFF CACHE BOOL "")
    include(FetchContent)
    FetchContent_Declare(tinyxml2
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/tinyxml2
        OVERRIDE_FIND_PACKAGE
    )

    # unfortunate hack to make TinyEXIF happy
    function(export)
    endfunction()

    set(BUILD_SHARED_LIBS ON CACHE BOOL "")
    set(BUILD_DEMO OFF CACHE BOOL "")
    FetchContent_Declare(TinyEXIF
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/TinyEXIF
        OVERRIDE_FIND_PACKAGE)
endif()

find_package(TinyEXIF CONFIG REQUIRED)
list(APPEND LIBRARY_DEPS TinyEXIF)


# OPENCV
if(BUILD_OPENCV)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/opencv)
    execute_process(
        COMMAND ${CMAKE_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/opencv
                                 -DCMAKE_INSTALL_PREFIX=opencv_install
                                 -DBUILD_LIST=core,imgproc,highgui,imgcodecs
                                 -DCMAKE_BUILD_TYPE=Release
                                 -DBUILD_EXAMPLES=OFF
                                 -DBUILD_TESTS=OFF
                                 -DWITH_OPENVINO=OFF
                                 -DBUILD_DOCS=OFF
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/opencv
    )

    execute_process(
        COMMAND ${CMAKE_COMMAND} --build . --parallel
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/opencv
    )

    execute_process(
        COMMAND ${CMAKE_COMMAND} --install .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/opencv
    )

    set(OpenCV_DIR "${CMAKE_BINARY_DIR}/opencv/opencv_install/lib/cmake/opencv4")
endif()

find_package(OpenCV CONFIG REQUIRED opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs)
list(APPEND LIBRARY_DEPS ${OpenCV_LIBS})


# GOOGLETEST
if(BUILD_TESTS)
    if(BUILD_GOOGLETEST)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/googletest)

        execute_process(
            COMMAND ${CMAKE_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/googletest
                -DCMAKE_INSTALL_PREFIX=googletest_install
                -DCMAKE_BUILD_TYPE=Release
                -DBUILD_GMOCK=OFF
                -Dgtest_force_shared_crt=ON
                -DBUILD_SHARED_LIBS=OFF
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest
        )

        execute_process(
            COMMAND ${CMAKE_COMMAND} --build . --parallel
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest
        )

        execute_process(
            COMMAND ${CMAKE_COMMAND} --install .
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest
        )

        set(GTest_DIR "${CMAKE_BINARY_DIR}/googletest/googletest_install/lib/cmake/GTest")
    endif()

    find_package(GTest 1.12.1 CONFIG REQUIRED)
    list(APPEND LIBRARY_DEPS GTest::gtest GTest::gtest_main)
endif()


# FLATBUFFERS
if(BUILD_FLATBUFFERS)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/flatbuffers)

    execute_process(
        COMMAND ${CMAKE_COMMAND}
            -DCMAKE_INSTALL_PREFIX=flatbuffers_install
            -DCMAKE_BUILD_TYPE=Release
            -DFLATBUFFERS_BUILD_FLATC=OFF
            -DFLATBUFFERS_BUILD_TESTS=OFF
            -DFLATBUFFERS_SKIP_MONSTER_EXTRA=ON
            -DFLATBUFFERS_ENABLE_PCH=ON
            ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/flatbuffers
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/flatbuffers
    )

    execute_process(
        COMMAND ${CMAKE_COMMAND} --build . --parallel
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/flatbuffers
    )

    execute_process(
        COMMAND ${CMAKE_COMMAND} --install .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/flatbuffers
    )

    set(FlatBuffers_DIR "${CMAKE_BINARY_DIR}/flatbuffers/flatbuffers_install/lib/cmake/flatbuffers")
endif()

find_package(FlatBuffers 25.2.10 CONFIG REQUIRED)
list(APPEND LIBRARY_DEPS flatbuffers::flatbuffers)

if(BUILD_FLATBUFFERS)
    include_directories(${CMAKE_BINARY_DIR}/flatbuffers/flatbuffers_install/include)
else()
    include_directories(${FlatBuffers_INCLUDE_DIRS})
endif()

if(USE_FLATBUFFERS_STRUCTURES)
    set(GEOTAGS_INCLUDE_FLATBUFFERS "${CMAKE_CURRENT_SOURCE_DIR}/include/fb_structs")
else()
    set(GEOTAGS_INCLUDE_FLATBUFFERS "${CMAKE_CURRENT_SOURCE_DIR}/include/not_fb_structs")
endif()
include_directories("${GEOTAGS_INCLUDE_FLATBUFFERS}")


# PYBIND11
if(BUILD_PYTHON_BINDINGS)
    if(BUILD_PYBIND11)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/pybind11)

        execute_process(
            COMMAND ${CMAKE_COMMAND}
                -DCMAKE_INSTALL_PREFIX=pybind11_install
                -DCMAKE_BUILD_TYPE=Release
                -DPYBIND11_TEST=OFF
                ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/pybind11
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/pybind11
        )

        execute_process(
            COMMAND ${CMAKE_COMMAND} --build . --parallel
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/pybind11
        )

        execute_process(
            COMMAND ${CMAKE_COMMAND} --install .
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/pybind11
        )

        set(pybind11_DIR "${CMAKE_BINARY_DIR}/pybind11/pybind11_install/share/cmake/pybind11")
    endif()

    find_package(pybind11 CONFIG REQUIRED)
    list(APPEND LIBRARY_DEPS pybind11::pybind11)

    include_directories(${pybind11_INCLUDE_DIRS})
endif()


# TG
list(APPEND LIBRARY_DEPS tg)

set(TG_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/tg")
include_directories("${TG_INCLUDE}")


# INCLUDES
set(GEOTAGS_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${GEOTAGS_INCLUDE}")

# BUILD
add_subdirectory(src)
add_subdirectory(samples)
add_subdirectory(3rdparty/tg)
if (BUILD_TESTS)
    add_subdirectory(tests)
endif()

# REPORT
message( STATUS "" )
message( STATUS "General configuration for ${PROJECT_NAME}" )
message( STATUS "======================================" )
message( STATUS "" )
message( STATUS "   Configuration:         ${CMAKE_BUILD_TYPE}" )
message( STATUS "   BUILD_TINYEXIF:        ${BUILD_TINYEXIF}" )
message( STATUS "   BUILD_OPENCV:          ${BUILD_OPENCV}" )
message( STATUS "   BUILD_FLATBUFFERS:     ${BUILD_FLATBUFFERS}" )
message( STATUS "   BUILD_GOOGLETEST:      ${BUILD_GOOGLETEST}" )
message( STATUS "   BUILD_TESTS:           ${BUILD_TESTS}" )
message( STATUS "   BUILD_PYBIND11:        ${BUILD_PYBIND11}" )
message( STATUS "   BUILD_PYTHON_BINDINGS: ${BUILD_PYTHON_BINDINGS}" )
message( STATUS "   LIBRARY_DEPS:          ${LIBRARY_DEPS}" )
message( STATUS "   GEOTAGS_INCLUDE:       ${GEOTAGS_INCLUDE}" )
message( STATUS "" )
