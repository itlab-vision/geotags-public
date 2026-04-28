set(target ${GEOTAGS_LIBRARY})
FILE(GLOB hdrs "*.h*" "${GEOTAGS_INCLUDE}/*.h*")
FILE(GLOB srcs "*.cpp")

ADD_LIBRARY(${target} STATIC ${hdrs} ${srcs})
TARGET_LINK_LIBRARIES(${target} ${LIBRARY_DEPS})

if(USE_FLATBUFFERS_STRUCTURES)
    add_subdirectory(fb_structs)
else()
    add_subdirectory(not_fb_structs)
endif()

# BUILD PYTHON BINDINGS
if(BUILD_PYTHON_BINDINGS)
    set(target geotags_module)

    file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
    file(GLOB binding_srcs "${CMAKE_CURRENT_SOURCE_DIR}/python_bindings/*.cpp")

    if(USE_FLATBUFFERS_STRUCTURES)
        file(GLOB fb_srcs "${CMAKE_CURRENT_SOURCE_DIR}/fb_structs/*.cpp")
    else()
        file(GLOB fb_srcs "${CMAKE_CURRENT_SOURCE_DIR}/not_fb_structs/*.cpp")
    endif()

    list(APPEND srcs ${fb_srcs} ${binding_srcs})

    pybind11_add_module(${target} ${srcs})

    target_link_libraries(${target} PRIVATE ${LIBRARY_DEPS})

    target_include_directories(${target} PRIVATE
        ${GEOTAGS_INCLUDE}
        ${GEOTAGS_INCLUDE_FLATBUFFERS}
        ${TG_INCLUDE}
    )

    install(TARGETS ${target} DESTINATION ${GEOTAGS_LIBRARY})
endif()
