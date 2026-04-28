// Copyright 2025 itlab-vision

#include <string>
#include <vector>

#include "coordinates.hpp"    // NOLINT
#include "find_district.hpp"  // NOLINT

NearestLocationState find_district(
    const std::vector<Pair<DistrictInfo, tg_geom *>> &districts,
    const Coordinates &location, DistrictInfo &district) {
    if (districts.size() == 0) {
        throw std::runtime_error("List of available locations is empty");
    }

    tg_geom *pointGeom =
        tg_geom_new_point(tg_point{location.longitude, location.latitude});

    for (int i = 0; i < districts.size(); i++) {
        tg_geom *geom = districts[i].second;

        if (tg_geom_contains(geom, pointGeom)) {
            tg_geom_free(pointGeom);
            if (i < districts.size()) {
                district = districts[i].first;
                return NearestLocationState::ExactLocation;
            }
        }
    }
    return NearestLocationState::ApproximateLocation;
}
