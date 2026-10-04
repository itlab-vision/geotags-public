// Copyright 2025 itlab-vision
#pragma once

#include <string>

#include "district_neighbors_reader_types.hpp"  // NOLINT
#include "generic_create_reader.hpp"            // NOLINT

// Factory method
inline DistrictNeighborsReaderPtr create_district_neighbors_reader(
    const std::string& reader_type, const std::string& file_name) {
#ifndef USE_FLATBUFFERS_STRUCTURES
    return generic_create_reader<DistrictNeighborsReaderSTL,
                                 DistrictNeighborsReaderFB,
                                 DistrictNeighborsReaderPtr>(reader_type,
                                                             file_name);
#else
    return generic_create_reader<DistrictNeighborsReaderFB,
                                 DistrictNeighborsReaderPtr>(reader_type,
                                                             file_name);
#endif
}
