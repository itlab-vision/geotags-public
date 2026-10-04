// Copyright 2025 itlab-vision
#pragma once

#include <string>

#include "districts_reader_types.hpp"  // NOLINT
#include "generic_create_reader.hpp"   // NOLINT

// Factory method
inline DistrictsReaderPtr create_districts_reader(
    const std::string& reader_type, const std::string& file_name) {
#ifndef USE_FLATBUFFERS_STRUCTURES
    return generic_create_reader<DistrictsReaderSTL, DistrictsReaderFB,
                                 DistrictsReaderPtr>(reader_type, file_name);
#else
    return generic_create_reader<DistrictsReaderFB, DistrictsReaderPtr>(
        reader_type, file_name);
#endif
}
