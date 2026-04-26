// Copyright 2025 itlab-vision

#include <algorithm>
#include <utility>
#include <string>
#include <vector>
#include <sstream>

#include "locations_segments_reader.hpp"  // NOLINT

LocationsSegmentsReader::LocationsSegmentsReader(const std::string& fname)
    : Reader(fname) {
    open_file();
}

std::vector<LocationsSegmentInfo> LocationsSegmentsReader::read() {
    std::string segments_size;
    std::getline(file, segments_size);

    std::vector<LocationsSegmentInfo> segments;
    segments.reserve(convert_num_argument<size_t>(segments_size));

    std::string attributes;
    std::getline(file, attributes);

    while (!file.eof()) {
        LocationsSegmentInfo segment;
        if (parse_line(segment)) {
            segments.push_back(segment);
        }
    }

    return segments;
}
