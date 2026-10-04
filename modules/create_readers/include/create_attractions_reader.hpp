// Copyright 2025 itlab-vision
#pragma once

#include <string>

#include "attractions_reader_types.hpp"  // NOLINT
#include "generic_create_reader.hpp"     // NOLINT

// Factory method
inline AttractionsReaderPtr create_attractions_reader(
    const std::string& reader_type, const std::string& file_name) {
#ifndef USE_FLATBUFFERS_STRUCTURES
    return generic_create_reader<AttractionsReaderSTL, AttractionsReaderFB,
                                 AttractionsReaderPtr>(reader_type, file_name);
#else
    return generic_create_reader<AttractionsReaderFB, AttractionsReaderPtr>(
        reader_type, file_name);
#endif
}
