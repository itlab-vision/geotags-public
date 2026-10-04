// Copyright 2025 itlab-vision

#include <sys/stat.h>
#include <algorithm>
#include <utility>
#include <string>
#include <vector>
#include <sstream>

#include "segments_parser.hpp"  // NOLINT

bool SegmentsParser::parse_line(SegmentInfo& segment, const std::string& str) {
    const std::vector<std::string>& result = separate_cells(str);
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

    segment =
        SegmentInfo(id, std::make_pair(lat_min, lat_max),
                    std::make_pair(lon_min, lon_max), neighbors, locations);
    return true;
}
