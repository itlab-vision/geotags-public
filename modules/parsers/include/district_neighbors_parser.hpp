// Copyright 2025 itlab-vision
#pragma once

#include <utility>
#include <vector>
#include <string>

#include "auxiliary.hpp"               // NOLINT
#include "parser_interface.hpp"        // NOLINT
#include "district_neighbor_info.hpp"  // NOLINT

class DistrictNeighborsParser : public ParserInterface<DistrictNeighborInfo> {
 public:
    void init_parser(const std::string& header) override {};
    bool parse_line(DistrictNeighborInfo& neighbor,
                    const std::string& str) override;

    DistrictNeighborsParser() = default;
};
