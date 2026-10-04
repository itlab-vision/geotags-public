// Copyright 2025 itlab-vision
#pragma once

#include <string>
#include <stdexcept>
#include <variant>

#include "auxiliary.hpp"  // NOLINT

#ifndef USE_FLATBUFFERS_STRUCTURES

template <typename STLReader, typename FBReader, typename ReaderPtr>
inline ReaderPtr generic_create_reader(const std::string& reader_type,
                                       const std::string& file_name) {
    if (reader_type == "csv") {
        return Ptr<STLReader>(new STLReader(file_name));
    }
    if (reader_type == "csv_fb") {
        return Ptr<FBReader>(new FBReader(file_name));
    }
    throw std::invalid_argument("Unknown reader type: " + reader_type);
}

#else

template <typename FBReader, typename ReaderPtr>
inline ReaderPtr generic_create_reader(const std::string& reader_type,
                                       const std::string& file_name) {
    if (reader_type == "csv_fb") {
        return Ptr<FBReader>(new FBReader(file_name));
    }
    throw std::invalid_argument("Unknown reader type: " + reader_type);
}

#endif
