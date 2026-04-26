// Copyright 2025 itlab-vision
#pragma once

#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include "district_neighbors_generated.hpp"  // NOLINT
#include "reader.hpp"                        // NOLINT

class DistrictNeighborsReader : public Reader {
 private:
    std::string fb_file_name;

    const flatbuffers::Vector<
        flatbuffers::Offset<DistrictNeighborsData::District>> *
    deserialize();
    void serialize();

 public:
    explicit DistrictNeighborsReader(const std::string &path);
    const flatbuffers::Vector<
        flatbuffers::Offset<DistrictNeighborsData::District>> *
    read();
};
