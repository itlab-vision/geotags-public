// Copyright 2025 itlab-vision

#include <string>
#include <vector>

#include "district_neighbors_parser.hpp"  // NOLINT

bool DistrictNeighborsParser::parse_line(DistrictNeighborInfo& neighbor,
                                         const std::string& str) {
    const std::vector<std::string>& result = separate_cells(str);
    if (result.empty()) {
        return false;
    }

    unsigned int id = convert_num_argument<unsigned int>(result[0]);
    std::vector<unsigned int> neighbors =
        convert_vec_argument<unsigned int>(result[1]);
    neighbor = DistrictNeighborInfo(id, neighbors);
    return true;
}
