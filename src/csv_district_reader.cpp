// Copyright 2025 itlab-vision

#include <algorithm>
#include <limits>
#include <string>
#include <utility>
#include <vector>
#include <sstream>

#include "district_reader.hpp"      // NOLINT
#include "coordinates.hpp"          // NOLINT
#include "district_info.hpp"        // NOLINT
#include "auxiliary.hpp"            // NOLINT
#include "districts_generated.hpp"  // NOLINT

CSVDistrictsReader::CSVDistrictsReader(const std::string &csv_path)
    : DistrictsReader(csv_path) {
    open_file();
}

bool CSVDistrictsReader::parse_line(
    std::pair<DistrictInfo, tg_geom *> &region) {
    std::vector<std::string> result = separate_cells();
    if (result.empty()) {
        return false;
    }

    unsigned int id = convert_num_argument<unsigned int>(result[0]);

    tg_geom *geom = tg_parse_wkt(result[3].c_str());
    const char *error = tg_geom_error(geom);
    if (error) {
        tg_geom_free(geom);
        throw std::invalid_argument("Invalid geometry field : " +
                                    std::string(error));
    }

    region = std::make_pair(DistrictInfo(id, result[1], result[2]), geom);
    return true;
}

std::vector<std::pair<DistrictInfo, tg_geom *>> CSVDistrictsReader::read() {
    std::vector<std::pair<DistrictInfo, tg_geom *>> regions;
    std::string attributes;
    std::getline(file, attributes);
    int i = 1;
    try {
        while (!file.eof()) {
            std::pair<DistrictInfo, tg_geom *> region =
                std::make_pair(DistrictInfo(), nullptr);
            if (parse_line(region)) {
                regions.push_back(region);
            }
            i++;
        }
    } catch (const std::exception &e) {
        for (auto region : regions) {
            tg_geom_free(region.second);
        }
        regions.clear();
        throw std::invalid_argument(std::string(e.what()) + " in " +
                                    std::to_string(i) + " line");
    }
    return regions;
}
