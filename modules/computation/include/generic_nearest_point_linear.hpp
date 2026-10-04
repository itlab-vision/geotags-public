// Copyright 2025 itlab-vision
#pragma once

#include <optional>

#include "auxiliary.hpp"              // NOLINT
#include "coordinates.hpp"            // NOLINT
#include "distance.hpp"               // NOLINT
#include "segments_reader_types.hpp"  // NOLINT
#include "generic_nearest_point.hpp"  // NOLINT

template <typename PointType, typename PointsWrapper>
class GenericNearestPointLinear
    : public GenericNearestPoint<PointType, PointsWrapper> {
 public:
    PrecisionState calculate(const PointsWrapper& points,
                             const std::optional<SegmentsWrapperType>& segments,
                             const Ptr<Distance>& dist_calculator,
                             const Coordinates& target_location,
                             PointType& nearest_point, double& min_distance,
                             const double threshold = 0.0) override;
};

template <typename PointType, typename PointsWrapper>
PrecisionState GenericNearestPointLinear<PointType, PointsWrapper>::calculate(
    const PointsWrapper& points,
    const std::optional<SegmentsWrapperType>& segments,
    const Ptr<Distance>& dist_calculator, const Coordinates& target_location,
    PointType& nearest_point, double& min_distance, const double threshold) {
    if (points.size() == 0) {
        throw std::runtime_error("List of available points is empty");
    }

    size_t best = 0;
    min_distance = dist_calculator->calculate(
        points.get_lat(0), points.get_lon(0), target_location.latitude,
        target_location.longitude);

    for (size_t i = 1; i < points.size(); ++i) {
        double distance = dist_calculator->calculate(
            points.get_lat(i), points.get_lon(i), target_location.latitude,
            target_location.longitude);
        if (distance < min_distance) {
            best = i;
            min_distance = distance;
        }
    }

    nearest_point = points.get_info(best);

    if (threshold > 0 && min_distance > threshold) {
        return PrecisionState::Approximate;
    }
    return PrecisionState::Exact;
}
