// Copyright 2025 itlab-vision
#pragma once

#include <utility>
#include <vector>
#include <string>

#include "parser_interface.hpp"  // NOLINT
#include "attraction_info.hpp"   // NOLINT
#include "coordinates.hpp"       // NOLINT

using AttractionItem = std::pair<AttractionInfo, Coordinates>;

class AttractionsParser : public ParserInterface<AttractionItem> {
 private:
    std::vector<std::string> extra_fields_names_;

 public:
    void init_parser(const std::string& header) override;
    bool parse_line(AttractionItem& attraction,
                    const std::string& str) override;

    AttractionsParser() = default;
};
