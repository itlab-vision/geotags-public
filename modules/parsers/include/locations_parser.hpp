// Copyright 2025 itlab-vision
#pragma once

#include <utility>
#include <vector>
#include <string>

#include "parser_interface.hpp"  // NOLINT
#include "location_info.hpp"     // NOLINT
#include "coordinates.hpp"       // NOLINT

using LocationItem = std::pair<LocationInfo, Coordinates>;

struct ParsedBase {
    unsigned int id;
    std::string country;
    std::string city;
    double lat;
    double lon;
};

class LocationsParser : public ParserInterface<LocationItem> {
 private:
    typedef bool (LocationsParser::*ParseFn)(LocationItem&, const std::string&);
    ParseFn parser_;

    ParsedBase parse_base(const std::vector<std::string>& cells);
    bool parse_line_basic(LocationItem& location, const std::string& str);
    bool parse_line_with_alt(LocationItem& location, const std::string& str);
    bool parse_line_with_ids_and_alt(LocationItem& location,
                                     const std::string& str);
    bool parse_line_with_attraction_db(LocationItem& location,
                                       const std::string& str);

 public:
    void init_parser(const std::string& header) override;
    bool parse_line(LocationItem& location, const std::string& str) override;

    LocationsParser() = default;
};
