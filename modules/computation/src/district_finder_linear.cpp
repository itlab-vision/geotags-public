// Copyright 2025 itlab-vision

#include "district_finder_linear.hpp"  // NOLINT

PrecisionState DistrictFinderLinear::find_district(
    const DistrictsWrapperType& districts,
    const std::optional<DistrictNeighborsWrapperType>& neighbors,
    const Coordinates& location, LocationInfo& nearest_location,
    DistrictInfo& district) {
    if (districts.size() == 0) {
        throw std::runtime_error("List of available districts is empty");
    }

    TgGeomPtr pointGeom(
        tg_geom_new_point(tg_point{location.longitude, location.latitude}));

    for (int i = 0; i < districts.size(); ++i) {
        const auto& geom = districts.get_geom(i);

        if (tg_geom_contains(geom.get(), pointGeom.get())) {
            district = districts.get_info(i);
            return PrecisionState::Exact;
        }
    }
    return PrecisionState::Approximate;
}
