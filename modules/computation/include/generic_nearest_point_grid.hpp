// Copyright 2025 itlab-vision
#pragma once

#include <algorithm>
#include <limits>
#include <vector>
#include <utility>
#include <optional>
#include <stdexcept>

#include "auxiliary.hpp"              // NOLINT
#include "segments_reader_types.hpp"  // NOLINT
#include "generic_nearest_point.hpp"  // NOLINT

template <typename PointType, typename PointsWrapper>
class GenericNearestPointGrid
    : public GenericNearestPoint<PointType, PointsWrapper> {
 protected:
    void update_bbox(const std::optional<SegmentsWrapperType>& segments,
                     unsigned int segment_id,
                     std::pair<double, double>& lon_range,
                     std::pair<double, double>& lat_range) const;
    int find_segment(const std::optional<SegmentsWrapperType>& segments,
                     const Coordinates& location) const;
    void init_search_area(std::vector<unsigned int>& search_segments,
                          const std::optional<SegmentsWrapperType>& segments,
                          unsigned int segment_id,
                          std::pair<double, double>& lon_range,
                          std::pair<double, double>& lat_range);
    void searchSegment(const std::optional<SegmentsWrapperType>& segments,
                       unsigned int segment_id, const PointsWrapper& points,
                       const Ptr<Distance>& dist_calculator,
                       const Coordinates& target_location,
                       PointType& nearest_point, double& min_distance,
                       int& checked, std::vector<unsigned int>& pts);
    void expand_neighbors(std::vector<uint32_t>& new_search_segments,
                          const std::vector<uint32_t>& search_segments,
                          const std::optional<SegmentsWrapperType>& segments,
                          const std::pair<double, double>& lon_range,
                          const std::pair<double, double>& lat_range);

 public:
    PrecisionState calculate(const PointsWrapper& points,
                             const std::optional<SegmentsWrapperType>& segments,
                             const Ptr<Distance>& dist_calculator,
                             const Coordinates& target_location,
                             PointType& nearest_point, double& min_distance,
                             const double threshold = 0.0) override;
};

template <typename PointType, typename PointsWrapper>
void GenericNearestPointGrid<PointType, PointsWrapper>::update_bbox(
    const std::optional<SegmentsWrapperType>& segments, unsigned int segment_id,
    std::pair<double, double>& lon_range,
    std::pair<double, double>& lat_range) const {
    lon_range.first =
        std::min(lon_range.first, segments->get_lon_min(segment_id));
    lon_range.second =
        std::max(lon_range.second, segments->get_lon_max(segment_id));
    lat_range.first =
        std::min(lat_range.first, segments->get_lat_min(segment_id));
    lat_range.second =
        std::max(lat_range.second, segments->get_lat_max(segment_id));
}

template <typename PointType, typename PointsWrapper>
int GenericNearestPointGrid<PointType, PointsWrapper>::find_segment(
    const std::optional<SegmentsWrapperType>& segments,
    const Coordinates& location) const {
    // Compute row size
    int row_size = 0;
    const double first_lat = segments->get_lat_min(0);
    for (size_t i = 0; i < segments->size(); ++i) {
        if (segments->get_lat_min(i) != first_lat) {
            break;
        }
        row_size++;
    }

    int segment_id = -1;

    // find column
    if (location.longitude > segments->get_lon_min(segments->size() - 1)) {
        segment_id = segments->get_segment_id(segments->size() - 1);
    } else if (location.longitude < segments->get_lon_max(0)) {
        segment_id = 0;
    } else {
        segment_id = 0;
        while (location.longitude >= segments->get_lon_max(segment_id)) {
            segment_id++;
        }
    }

    // adjust row
    if (location.latitude < segments->get_lat_min(segment_id)) {
        while (segment_id - row_size >= 0 &&
               segments->get_lat_min(segment_id) > location.latitude) {
            segment_id -= row_size;
        }
    } else {
        while (segment_id + row_size < segments->size() &&
               segments->get_lat_max(segment_id) < location.latitude) {
            segment_id += row_size;
        }
    }

    return segment_id;
}

template <typename PointType, typename PointsWrapper>
void GenericNearestPointGrid<PointType, PointsWrapper>::init_search_area(
    std::vector<unsigned int>& search_segments,
    const std::optional<SegmentsWrapperType>& segments, unsigned int segment_id,
    std::pair<double, double>& lon_range,
    std::pair<double, double>& lat_range) {
    segments->get_nbrs(segment_id, search_segments);
    search_segments.push_back(segment_id);

    lon_range.first = std::numeric_limits<double>::max();
    lon_range.second = std::numeric_limits<double>::lowest();
    lat_range.first = std::numeric_limits<double>::max();
    lat_range.second = std::numeric_limits<double>::lowest();
    for (auto seg_id : search_segments) {
        update_bbox(segments, seg_id, lon_range, lat_range);
    }
}

template <typename PointType, typename PointsWrapper>
void GenericNearestPointGrid<PointType, PointsWrapper>::searchSegment(
    const std::optional<SegmentsWrapperType>& segments, unsigned int segment_id,
    const PointsWrapper& points, const Ptr<Distance>& dist_calculator,
    const Coordinates& target_location, PointType& nearest_point,
    double& min_distance, int& checked, std::vector<unsigned int>& pts) {
    segments->get_points(segment_id, pts);
    if (pts.empty()) return;

    unsigned int best = -1;

    for (unsigned int loc_id : pts) {
        double distance = dist_calculator->calculate(
            points.get_lat(loc_id), points.get_lon(loc_id),
            target_location.latitude, target_location.longitude);
        if (distance < min_distance) {
            min_distance = distance;
            best = loc_id;
        }
        checked++;
    }

    if (best != -1) {
        nearest_point = points.get_info(best);
    }
}

template <typename PointType, typename PointsWrapper>
void GenericNearestPointGrid<PointType, PointsWrapper>::expand_neighbors(
    std::vector<uint32_t>& new_search_segments,
    const std::vector<uint32_t>& search_segments,
    const std::optional<SegmentsWrapperType>& segments,
    const std::pair<double, double>& lon_range,
    const std::pair<double, double>& lat_range) {
    std::vector<unsigned int> nbrs;
    for (auto seg_id : search_segments) {
        segments->get_nbrs(seg_id, nbrs);
        for (auto neighbor_id : nbrs) {
            if ((lon_range.second <= segments->get_lon_min(neighbor_id) ||
                 lon_range.first >= segments->get_lon_max(neighbor_id) ||
                 lat_range.second <= segments->get_lat_min(neighbor_id) ||
                 lat_range.first >= segments->get_lat_max(neighbor_id)) &&
                std::find(new_search_segments.begin(),
                          new_search_segments.end(),
                          neighbor_id) == new_search_segments.end()) {
                new_search_segments.push_back(neighbor_id);
            }
        }
    }
}

template <typename PointType, typename PointsWrapper>
PrecisionState GenericNearestPointGrid<PointType, PointsWrapper>::calculate(
    const PointsWrapper& points,
    const std::optional<SegmentsWrapperType>& segments,
    const Ptr<Distance>& dist_calculator, const Coordinates& target_location,
    PointType& nearest_point, double& min_distance, const double threshold) {
    if (points.size() == 0) {
        throw std::runtime_error("List of available locations is empty");
    }
    if (segments->size() == 0) {
        throw std::runtime_error("Grid segments are not available");
    }

    // Find segment containing the target_location
    int segment_id = find_segment(segments, target_location);
    if (segment_id < 0 || segment_id >= segments->size()) {
        throw std::runtime_error(
            "Failed to find grid segment for the location");
    }

    // Initialize search area (neighbors + current)
    std::vector<unsigned int> search_segments;
    std::pair<double, double> lon_range, lat_range;
    init_search_area(search_segments, segments, segment_id, lon_range,
                     lat_range);

    // Search nearest location in segments
    min_distance = std::numeric_limits<double>::max();
    int checked = 0;

    std::vector<unsigned int> pts;
    while (true) {
        for (auto seg_id : search_segments) {
            searchSegment(segments, seg_id, points, dist_calculator,
                          target_location, nearest_point, min_distance, checked,
                          pts);
        }

        if (checked != 0) break;

        std::vector<uint32_t> new_search_segments;
        expand_neighbors(new_search_segments, search_segments, segments,
                         lon_range, lat_range);

        if (new_search_segments.empty()) break;
        search_segments = std::move(new_search_segments);

        for (auto seg_id : search_segments) {
            update_bbox(segments, seg_id, lon_range, lat_range);
        }
    }

    if (checked == 0) {
        throw std::runtime_error(
            "Failed to find any location in the search segments");
    }

    // Check threshold
    if (threshold > 0 && min_distance > threshold) {
        return PrecisionState::Approximate;
    }
    return PrecisionState::Exact;
}
