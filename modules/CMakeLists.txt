# Collect public include directories from all modules.
set(GEOTAGS_MODULE_INCLUDE_DIRS)
file(GLOB GEOTAGS_MODULE_INCLUDE_DIRS
    LIST_DIRECTORIES true
    "${CMAKE_CURRENT_SOURCE_DIR}/*/include"
)

# Collect library sources and exclude Python binding sources.
file(GLOB_RECURSE GEOTAGS_LIBRARY_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*/src/*.cpp"
)
list(FILTER GEOTAGS_LIBRARY_SOURCES EXCLUDE REGEX
    "/python_bindings/src/"
)

# Add sources from the selected data structure implementation.
if(USE_FLATBUFFERS_STRUCTURES)
    file(GLOB_RECURSE GEOTAGS_VARIANT_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/*/src/fb_structs/*.cpp"
    )
else()
    file(GLOB_RECURSE GEOTAGS_VARIANT_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/*/src/not_fb_structs/*.cpp"
    )
endif()
list(APPEND GEOTAGS_LIBRARY_SOURCES ${GEOTAGS_VARIANT_SOURCES})

# Collect module headers for the library target.
file(GLOB_RECURSE GEOTAGS_LIBRARY_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*/include/*.hpp"
)

add_library(${GEOTAGS_LIBRARY} STATIC
    ${GEOTAGS_LIBRARY_SOURCES}
    ${GEOTAGS_LIBRARY_HEADERS}
)

# Propagate dependencies and include paths to library consumers.
target_link_libraries(${GEOTAGS_LIBRARY} PUBLIC ${LIBRARY_DEPS})
target_include_directories(${GEOTAGS_LIBRARY} PUBLIC
    ${GEOTAGS_MODULE_INCLUDE_DIRS}
    ${GEOTAGS_INCLUDE_FLATBUFFERS}
    "${CMAKE_CURRENT_SOURCE_DIR}/fb_generated"
)

# Build Python bindings as a separate module linked to the main library.
if(BUILD_PYTHON_BINDINGS)
    file(GLOB PYTHON_BINDING_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/python_bindings/src/*.cpp"
    )

    pybind11_add_module(geotags_module ${PYTHON_BINDING_SOURCES})
    target_link_libraries(geotags_module PRIVATE
        ${GEOTAGS_LIBRARY}
        sanitizers
        ${LIBRARY_DEPS}
    )

    install(TARGETS geotags_module DESTINATION ${GEOTAGS_LIBRARY})
endif()
