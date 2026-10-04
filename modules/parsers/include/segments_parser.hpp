// Copyright 2025 itlab-vision
#pragma once

#include <utility>
#include <vector>
#include <string>

#include "parser_interface.hpp"  // NOLINT
#include "segment_info.hpp"      // NOLINT

class SegmentsParser : public ParserInterface<SegmentInfo> {
 public:
    void init_parser(const std::string& header) override {};
    bool parse_line(SegmentInfo& segment, const std::string& str) override;

    SegmentsParser() = default;
};
