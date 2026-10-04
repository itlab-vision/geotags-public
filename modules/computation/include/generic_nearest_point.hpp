// Copyright 2025 itlab-vision
#pragma once

#include <string>
#include <optional>

#include "auxiliary.hpp"              // NOLINT
#include "coordinates.hpp"            // NOLINT
#include "distance.hpp"               // NOLINT
#include "segments_reader_types.hpp"  // NOLINT

template <typename PointType, typename PointsWrapper>
class GenericNearestPoint {
 public:
    virtual ~GenericNearestPoint() = default;

    virtual PrecisionState calculate(
        const PointsWrapper& points,
        const std::optional<SegmentsWrapperType>& segments,
        const Ptr<Distance>& dist_calculator,
        const Coordinates& target_location, PointType& nearest_point,
        double& min_distance, const double threshold = 0.0) = 0;

    static Ptr<GenericNearestPoint> create(const std::string& search_type);
};

#include "generic_nearest_point.tpp"  // NOLINT
