// Copyright 2025 itlab-vision

#include <algorithm>
#include <limits>
#include <string>
#include <utility>
#include <vector>
#include <sstream>

#include "locations_reader.hpp"  // NOLINT

CSVLocationsReader::CSVLocationsReader(const std::string &csv_path)
    : LocationsReader(csv_path), CSVLocationParser(csv_path), Reader(csv_path) {
    open_file();
}

std::vector<std::pair<LocationInfo, Coordinates>> CSVLocationsReader::read() {
    std::string locations_size;
    std::getline(file, locations_size);

    std::vector<std::pair<LocationInfo, Coordinates>> locations;
    locations.reserve(convert_num_argument<size_t>(locations_size));

    std::string attributes;
    std::getline(file, attributes);
    detect_parser_format(attributes);

    while (!file.eof()) {
        std::pair<LocationInfo, Coordinates> location = std::make_pair(
            LocationInfo(), Coordinates(std::numeric_limits<double>::max(),
                                        std::numeric_limits<double>::max()));
        if (parse_line(location)) {
            locations.push_back(location);
        }
    }

    return locations;
}
