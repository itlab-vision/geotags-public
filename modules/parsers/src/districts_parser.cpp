// Copyright 2025 itlab-vision

#include <string>
#include <vector>
#include <utility>

#include "districts_parser.hpp"  // NOLINT

bool DistrictsParser::parse_line(DistrictItem& district,
                                 const std::string& str) {
    const std::vector<std::string>& result = separate_cells(str);
    if (result.empty()) {
        return false;
    }

    unsigned int id = convert_num_argument<unsigned int>(result[0]);

    TgGeomPtr geom(tg_parse_wkt(result[3].c_str()));
    const char* error = tg_geom_error(geom.get());
    if (error) {
        throw std::invalid_argument("Invalid geometry field : " +
                                    std::string(error));
    }

    district = std::make_pair(DistrictInfo(id, result[1], result[2]),
                              TgGeomSharedPtr(std::move(geom)));
    return true;
}
