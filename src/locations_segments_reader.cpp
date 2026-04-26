// Copyright 2025 itlab-vision

#include <sys/stat.h>
#include <algorithm>
#include <utility>
#include <string>
#include <vector>
#include <sstream>

#include "locations_segments_reader.hpp"  // NOLINT

bool LocationsSegmentsReader::parse_line(LocationsSegmentInfo& segment) {
    std::vector<std::string> result = separate_cells();
    if (result.empty()) {
        return false;
    }

    unsigned int id = convert_num_argument<unsigned int>(result[0]);
    double lat_min = convert_num_argument<double>(result[1]);
    double lat_max = convert_num_argument<double>(result[2]);
    double lon_min = convert_num_argument<double>(result[3]);
    double lon_max = convert_num_argument<double>(result[4]);
    std::vector<unsigned int> neighbors =
        convert_vec_argument<unsigned int>(result[5]);
    std::vector<unsigned int> locations =
        convert_vec_argument<unsigned int>(result[6]);

    segment = LocationsSegmentInfo(id, std::make_pair(lat_min, lat_max),
                                   std::make_pair(lon_min, lon_max), neighbors,
                                   locations);
    return true;
}
