// Copyright 2025 itlab-vision
#include <stdexcept>
#include <string>

#include "auxiliary.hpp"                          // NOLINT
#include "generic_nearest_point.hpp"              // NOLINT
#include "generic_nearest_point_linear.hpp"       // NOLINT
#include "generic_nearest_point_grid.hpp"         // NOLINT
#include "generic_nearest_point_grid_binary.hpp"  // NOLINT

template <typename PointType, typename PointsWrapper>
Ptr<GenericNearestPoint<PointType, PointsWrapper>>
GenericNearestPoint<PointType, PointsWrapper>::create(
    const std::string& search_type) {
    if (search_type == "linear") {
        return Ptr<GenericNearestPointLinear<PointType, PointsWrapper>>(
            new GenericNearestPointLinear<PointType, PointsWrapper>());
    }
    if (search_type == "grid") {
        return Ptr<GenericNearestPointGrid<PointType, PointsWrapper>>(
            new GenericNearestPointGrid<PointType, PointsWrapper>());
    }
    if (search_type == "grid_binary") {
        return Ptr<GenericNearestPointGridBinary<PointType, PointsWrapper>>(
            new GenericNearestPointGridBinary<PointType, PointsWrapper>());
    }
    throw std::invalid_argument("Unknown search type: " + search_type);
}
