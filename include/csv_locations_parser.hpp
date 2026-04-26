// Copyright 2025 itlab-vision
#pragma once

#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include "coordinates.hpp"    // NOLINT
#include "location_info.hpp"  // NOLINT
#include "auxiliary.hpp"      // NOLINT
#include "reader.hpp"         // NOLINT

struct ParsedBase {
    unsigned int id;
    std::string country;
    std::string city;
    double lat;
    double lon;
};

class CSVLocationParser : virtual public Reader {
 private:
    typedef bool (CSVLocationParser::*ParseFn)(
        std::pair<LocationInfo, Coordinates>&);
    ParseFn parser_;

    ParsedBase parse_base(const std::vector<std::string>& cells);
    bool parse_line_basic(std::pair<LocationInfo, Coordinates>& location);
    bool parse_line_with_alt(std::pair<LocationInfo, Coordinates>& location);
    bool parse_line_with_ids_and_alt(
        std::pair<LocationInfo, Coordinates>& location);

 protected:
    void detect_parser_format(const std::string& header);

    bool parse_line(std::pair<LocationInfo, Coordinates>& location);

    explicit CSVLocationParser(const std::string& fname) : Reader(fname) {}
};
