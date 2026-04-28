// Copyright 2025 itlab-vision

#include <vector>
#include <string>
#include <utility>

#include "csv_locations_parser.hpp"  // NOLINT

void CSVLocationParser::detect_parser_format(const std::string& header) {
    int cells_count = std::count(header.begin(), header.end(), ';') + 1;
    if (cells_count == 5) {
        parser_ = &CSVLocationParser::parse_line_basic;
    } else if (cells_count == 6) {
        parser_ = &CSVLocationParser::parse_line_with_alt;
    } else {
        parser_ = &CSVLocationParser::parse_line_with_ids_and_alt;
    }
}

ParsedBase CSVLocationParser::parse_base(
    const std::vector<std::string>& cells) {
    return ParsedBase{convert_num_argument<unsigned int>(cells[0]), cells[1],
                      cells[2], convert_num_argument<double>(cells[3]),
                      convert_num_argument<double>(cells[4])};
}

bool CSVLocationParser::parse_line_basic(
    std::pair<LocationInfo, Coordinates>& location) {
    std::vector<std::string> result = separate_cells();
    if (result.empty()) {
        return false;
    }

    auto base = parse_base(result);

    location = std::make_pair(
        LocationInfo(base.id, base.country, base.city, -1, -1, {}),
        Coordinates(base.lat, base.lon));

    return true;
}

bool CSVLocationParser::parse_line_with_alt(
    std::pair<LocationInfo, Coordinates>& location) {
    std::vector<std::string> result = separate_cells();
    if (result.empty()) {
        return false;
    }

    auto base = parse_base(result);

    std::vector<std::string> alt = convert_vec_argument<std::string>(result[5]);

    location = std::make_pair(
        LocationInfo(base.id, base.country, base.city, -1, -1, alt),
        Coordinates(base.lat, base.lon));

    return true;
}

bool CSVLocationParser::parse_line_with_ids_and_alt(
    std::pair<LocationInfo, Coordinates>& location) {
    std::vector<std::string> result = separate_cells();
    if (result.empty()) {
        return false;
    }

    auto base = parse_base(result);

    int region_id = -1;
    int district_id = -1;
    std::vector<std::string> alt_names;

    // TODO(TepidmishA): optimize - remove all extra checks
    {
        region_id = convert_num_argument<int>(result[5]);

        int next_col_idx = 6;

        // Check if the next argument is a district_id
        if (next_col_idx < result.size() &&
            try_convert_num_argument(result[next_col_idx], district_id)) {
            next_col_idx++;
        }

        // Parse alternative names
        if (next_col_idx < result.size()) {
            alt_names = convert_vec_argument<std::string>(result[next_col_idx]);
        }
    }

    location = std::make_pair(LocationInfo(base.id, base.country, base.city,
                                           region_id, district_id, alt_names),
                              Coordinates(base.lat, base.lon));

    return true;
}

bool CSVLocationParser::parse_line(
    std::pair<LocationInfo, Coordinates>& location) {
    return (this->*parser_)(location);
}
