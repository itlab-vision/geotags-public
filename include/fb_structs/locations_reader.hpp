// Copyright 2025 itlab-vision
#pragma once

#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include "coordinates.hpp"           // NOLINT
#include "location_info.hpp"         // NOLINT
#include "reader.hpp"                // NOLINT
#include "csv_locations_parser.hpp"  // NOLINT

#include "locations_generated.hpp"  // NOLINT

class CSVLocationsReaderFB : public CSVLocationParser {
 private:
    void prepare_file();
    const flatbuffers::Vector<flatbuffers::Offset<LocationsData::Location>> *
    deserialize();
    void serialize();

 public:
    explicit CSVLocationsReaderFB(const std::string &csv_path);
    const flatbuffers::Vector<flatbuffers::Offset<LocationsData::Location>> *
    read();
};
