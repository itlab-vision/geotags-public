// Copyright 2025 itlab-vision
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <utility>

#include "coordinates.hpp"             // NOLINT
#include "locations_segment_info.hpp"  // NOLINT
#include "auxiliary.hpp"               // NOLINT
#include "reader.hpp"                  // NOLINT

#include "locations_segments_generated.hpp"  // NOLINT

class LocationsSegmentsReader : public Reader {
 protected:
    bool parse_line(LocationsSegmentInfo &segment);

    const flatbuffers::Vector<
        flatbuffers::Offset<LocationsSegmentsData::LocationsSegmentInfo>> *
    deserialize();
    void serialize();

 public:
    explicit LocationsSegmentsReader(const std::string &fname);
    const flatbuffers::Vector<
        flatbuffers::Offset<LocationsSegmentsData::LocationsSegmentInfo>> *
    read();
};
