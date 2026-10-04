// Copyright 2025 itlab-vision

#include <vector>
#include <string>
#include <utility>
#include <unordered_map>

#include "attractions_parser.hpp"  // NOLINT

void AttractionsParser::init_parser(const std::string& header) {
    std::vector<std::string> result =
        convert_vec_argument<std::string>(header, ';');
    if (result.size() > 4) {
        extra_fields_names_.assign(result.begin() + 4, result.end());
    }
}

bool AttractionsParser::parse_line(AttractionItem& attraction,
                                   const std::string& str) {
    const std::vector<std::string>& result = separate_cells(str);
    if (result.empty()) {
        return false;
    }

    double lon = convert_num_argument<double>(result[0]);
    double lat = convert_num_argument<double>(result[1]);
    std::string type = result[2];
    std::string name = result[3];

    std::unordered_map<std::string, std::string> extra_fields;
    for (size_t i = 0; i < extra_fields_names_.size(); i++) {
        extra_fields[extra_fields_names_[i]] = result[4 + i];
    }

    attraction = std::make_pair(AttractionInfo(type, name, extra_fields),
                                Coordinates(lat, lon));

    return true;
}
