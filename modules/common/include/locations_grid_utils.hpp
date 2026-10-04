// Copyright 2025 itlab-vision
#pragma once

#include <vector>
#include <tuple>
#include <utility>
#include <algorithm>
#include <cmath>
#include <limits>

#include "segment_info.hpp"   // NOLINT
#include "location_info.hpp"  // NOLINT

namespace detail {

template <typename PointsWrapper>
void init_bbox(const PointsWrapper& points, double& lat_min, double& lat_max,
               double& lon_min, double& lon_max) {
    lat_min = points.get_lat(0);
    lat_max = points.get_lat(0);
    lon_min = points.get_lon(0);
    lon_max = points.get_lon(0);

    for (size_t i = 0; i < points.size(); ++i) {
        lat_min = std::min(lat_min, points.get_lat(i));
        lat_max = std::max(lat_max, points.get_lat(i));
        lon_min = std::min(lon_min, points.get_lon(i));
        lon_max = std::max(lon_max, points.get_lon(i));
    }
}

inline std::vector<SegmentInfo> build_segments(int rows, int cols,
                                               double lat_min, double lon_min,
                                               double cell_size) {
    std::vector<SegmentInfo> segments;
    segments.reserve(rows * cols);

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            unsigned int segment_id = r * cols + c;

            std::pair<double, double> lat_range = {
                lat_min + r * cell_size, lat_min + (r + 1) * cell_size};

            std::pair<double, double> lon_range = {
                lon_min + c * cell_size, lon_min + (c + 1) * cell_size};

            segments.emplace_back(segment_id, lat_range, lon_range,
                                  std::vector<unsigned int>{},
                                  std::vector<unsigned int>{});
        }
    }

    return segments;
}

template <typename PointsWrapper>
void assign_points_to_segments(const PointsWrapper& points,
                               std::vector<SegmentInfo>& segments, int rows,
                               int cols, double lat_min, double lon_min,
                               double cell_size) {
    for (size_t i = 0; i < points.size(); ++i) {
        int r = std::min(
            static_cast<int>((points.get_lat(i) - lat_min) / cell_size),
            rows - 1);

        int c = std::min(
            static_cast<int>((points.get_lon(i) - lon_min) / cell_size),
            cols - 1);

        LocationInfo loc_info(points.get_info(i));
        segments[r * cols + c].points.push_back(loc_info.location_id);
    }
}

template <typename PointsWrapper>
void sort_segment_locations(std::vector<SegmentInfo>& segments,
                            const PointsWrapper& points) {
    for (auto& segment : segments) {
        std::sort(segment.points.begin(), segment.points.end(),
                  [&](unsigned int a, unsigned int b) {
                      return points.get_lat(a) < points.get_lat(b);
                  });
    }
}

inline void compute_neighbors(std::vector<SegmentInfo>& segments, int rows,
                              int cols) {
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            unsigned int segment_id = r * cols + c;
            if (r > 0)
                segments[segment_id].neighbors.push_back((r - 1) * cols + c);
            if (r + 1 < rows)
                segments[segment_id].neighbors.push_back((r + 1) * cols + c);
            if (c > 0)
                segments[segment_id].neighbors.push_back(r * cols + (c - 1));
            if (c + 1 < cols)
                segments[segment_id].neighbors.push_back(r * cols + (c + 1));
        }
    }
}

template <typename PointsWrapper>
std::vector<int> count_points_in_grid(const PointsWrapper& points, int rows,
                                      int cols, double lat_min, double lon_min,
                                      double cell_size) {
    std::vector<int> counts(rows * cols, 0);

    for (size_t i = 0; i < points.size(); ++i) {
        int r = std::min(
            static_cast<int>((points.get_lat(i) - lat_min) / cell_size),
            rows - 1);

        int c = std::min(
            static_cast<int>((points.get_lon(i) - lon_min) / cell_size),
            cols - 1);

        counts[r * cols + c]++;
    }
    return counts;
}

inline std::tuple<double, int, int> compute_grid_stats(
    std::vector<int> counts) {
    std::sort(counts.begin(), counts.end());

    auto it_nonzero =
        std::find_if(counts.begin(), counts.end(), [](int v) { return v > 0; });
    if (it_nonzero == counts.end()) return {0.0, 0, 0};

    size_t start = std::distance(counts.begin(), it_nonzero);
    size_t m = counts.size() - start;

    double median = counts[start + (m - 1) / 2];
    int maxc = counts.back();
    int minc = *it_nonzero;

    return {median, maxc, minc};
}

inline double compute_grid_score(double median, int maxc, int rows, int cols,
                                 int total_locations) {
    double penalty_max =
        (static_cast<double>(maxc) /
         std::max(1.0, std::sqrt(static_cast<double>(total_locations)))) *
        0.5;

    double size_penalty = static_cast<double>(rows * cols) * 0.001;

    return median + penalty_max + size_penalty;
}

}  // namespace detail

template <typename PointsWrapper>
std::tuple<std::vector<SegmentInfo>, int, int> compute_grid(
    const PointsWrapper& points, double cell_size) {
    if (points.size() == 0) return {{}, 0, 0};

    double lat_min, lat_max, lon_min, lon_max;
    detail::init_bbox(points, lat_min, lat_max, lon_min, lon_max);

    int rows = static_cast<int>(std::ceil((lat_max - lat_min) / cell_size));
    int cols = static_cast<int>(std::ceil((lon_max - lon_min) / cell_size));

    auto segments =
        detail::build_segments(rows, cols, lat_min, lon_min, cell_size);
    detail::assign_points_to_segments(points, segments, rows, cols, lat_min,
                                      lon_min, cell_size);
    detail::sort_segment_locations(segments, points);
    detail::compute_neighbors(segments, rows, cols);

    return {std::move(segments), cols, rows};
}

template <typename PointsWrapper>
double compute_optimal_cell_size(const PointsWrapper& points,
                                 int max_grid_size = 100) {
    if (points.size() == 0) return 0.0;

    double lat_min, lat_max, lon_min, lon_max;
    detail::init_bbox(points, lat_min, lat_max, lon_min, lon_max);

    double lat_range = lat_max - lat_min;
    double lon_range = lon_max - lon_min;
    double min_range = std::min(lat_range, lon_range);

    double best_score = std::numeric_limits<double>::max();
    double best_cell_size = 0;

    int curr_grid_size = 2;
    while (curr_grid_size <= max_grid_size) {
        double cell_size = min_range / curr_grid_size;
        int rows = static_cast<int>(std::ceil(lat_range / cell_size));
        int cols = static_cast<int>(std::ceil(lon_range / cell_size));

        auto counts = detail::count_points_in_grid(points, rows, cols, lat_min,
                                                   lon_min, cell_size);
        auto [median, maxc, minc] = detail::compute_grid_stats(counts);

        double score =
            detail::compute_grid_score(median, maxc, rows, cols, points.size());
        if (score < best_score) {
            best_score = score;
            best_cell_size = cell_size;
        }

        curr_grid_size++;
    }

    return best_cell_size;
}
