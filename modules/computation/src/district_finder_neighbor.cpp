// Copyright 2025 itlab-vision

#include <utility>
#include <vector>

#include "district_finder_neighbor.hpp"  // NOLINT

PrecisionState DistrictFinderNeighbor::find_district(
    const DistrictsWrapperType& districts,
    const std::optional<DistrictNeighborsWrapperType>& neighbors,
    const Coordinates& location, LocationInfo& nearest_location,
    DistrictInfo& district) {
    if (districts.size() == 0) {
        throw std::runtime_error("List of available districts is empty");
    }

    if (neighbors->size() == 0) {
        throw std::runtime_error(
            "List of available neighbors districts is empty");
    }

    TgGeomPtr pointGeom(
        tg_geom_new_point(tg_point{location.longitude, location.latitude}));

    const auto& geom_base = districts.get_geom(nearest_location.region_id);
    if (tg_geom_contains(geom_base.get(), pointGeom.get())) {
        district = districts.get_info(nearest_location.region_id);
        return PrecisionState::Exact;
    }

    std::vector<unsigned int> neighbors_districts;
    neighbors->get_nbrs(nearest_location.region_id, neighbors_districts);
    for (const auto neighbors_id : neighbors_districts) {
        const auto& geom = districts.get_geom(neighbors_id);
        if (tg_geom_contains(geom.get(), pointGeom.get())) {
            district = districts.get_info(neighbors_id);
            return PrecisionState::Exact;
        }
    }

    return PrecisionState::Approximate;
}
