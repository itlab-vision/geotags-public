# get all cpp-files in current directory
file(GLOB samples_list RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

set(no_flatbuffers_samples
  search_admin_unit_app.cpp
  search_districts_app.cpp
  cities_grid_app.cpp
)

if(USE_FLATBUFFERS_STRUCTURES)
  list(REMOVE_ITEM samples_list ${no_flatbuffers_samples})
endif()

foreach(sample_filename ${samples_list})
  # get file name without extension
  get_filename_component(sample ${sample_filename} NAME_WE)

  # add and configure executable file to be produced 
  add_executable(${sample} ${sample_filename})
  target_link_libraries(${sample} ${GEOTAGS_LIBRARY})
  set_target_properties(${sample} PROPERTIES
    OUTPUT_NAME "${sample}"
    PROJECT_LABEL "${sample}"
    RUNTIME_OUTPUT_DIRECTORY "../")
endforeach()