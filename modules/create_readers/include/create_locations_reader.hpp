// Copyright 2025 itlab-vision
#pragma once

#include <string>

#include "locations_reader_types.hpp"  // NOLINT
#include "generic_create_reader.hpp"   // NOLINT

// Factory method
inline LocationsReaderPtr create_locations_reader(
    const std::string& reader_type, const std::string& file_name) {
#ifndef USE_FLATBUFFERS_STRUCTURES
    return generic_create_reader<LocationsReaderSTL, LocationsReaderFB,
                                 LocationsReaderPtr>(reader_type, file_name);
#else
    return generic_create_reader<LocationsReaderFB, LocationsReaderPtr>(
        reader_type, file_name);
#endif
}
