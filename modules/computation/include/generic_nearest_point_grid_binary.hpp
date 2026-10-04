// Copyright 2025 itlab-vision
#pragma once

#include <cmath>
#include <optional>
#include <vector>

#include "auxiliary.hpp"                   // NOLINT
#include "coordinates.hpp"                 // NOLINT
#include "distance.hpp"                    // NOLINT
#include "generic_nearest_point_grid.hpp"  // NOLINT

template <typename PointType, typename PointsWrapper>
class GenericNearestPointGridBinary
    : public GenericNearestPointGrid<PointType, PointsWrapper> {
 private:
    void searchSegment(const std::optional<SegmentsWrapperType>& segments,
                       int segment_idx, const PointsWrapper& points,
                       const Ptr<Distance>& dist_calculator,
                       const Coordinates& target_location,
                       PointType& nearest_point, double& min_distance,
                       int& checked, std::vector<unsigned int>& pts);
};

template <typename PointType, typename PointsWrapper>
void GenericNearestPointGridBinary<PointType, PointsWrapper>::searchSegment(
    const std::optional<SegmentsWrapperType>& segments, int segment_idx,
    const PointsWrapper& points, const Ptr<Distance>& dist_calculator,
    const Coordinates& target_location, PointType& nearest_point,
    double& min_distance, int& checked, std::vector<unsigned int>& pts) {
    segments->get_points(segment_idx, pts);
    auto size = pts.size();
    if (size == 0) return;

    const double target_lat = target_location.latitude;

    auto process_index = [&](int& idx, int step) {
        if (idx < 0 || idx >= size) return false;

        unsigned int loc_id = pts[idx];

        double lat_diff = std::abs(points.get_lat(loc_id) - target_lat);
        if (lat_diff > min_distance) return false;

        checked++;

        double d = dist_calculator->calculate(
            points.get_lat(loc_id), points.get_lon(loc_id),
            target_location.latitude, target_location.longitude);

        if (d < min_distance) {
            min_distance = d;
            nearest_point = points.get_info(loc_id);
        }

        idx += step;
        return true;
    };

    // Init borders (binary search by latitude)
    int low = 0;
    int high = size;
    while (low < high) {
        int mid = low + (high - low) / 2;
        unsigned int mid_loc_id = pts[mid];

        if (points.get_lat(mid_loc_id) < target_lat) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }

    int right = low;
    int left = right - 1;

    // Expand left/right while within min_distance
    while (left >= 0 || right < size) {
        bool updated = false;

        updated |= process_index(left, -1);
        updated |= process_index(right, 1);

        if (!updated) break;
    }
}
