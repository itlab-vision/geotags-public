// Copyright 2025 itlab-vision
#pragma once

#include <utility>
#include <vector>
#include <string>

#include "auxiliary.hpp"         // NOLINT
#include "parser_interface.hpp"  // NOLINT
#include "district_info.hpp"     // NOLINT

extern "C" {
#include "tg.h"  // NOLINT
}

using DistrictItem = std::pair<DistrictInfo, TgGeomSharedPtr>;

class DistrictsParser : public ParserInterface<DistrictItem> {
 public:
    void init_parser(const std::string& header) override {};
    bool parse_line(DistrictItem& district, const std::string& str) override;

    DistrictsParser() = default;
};
