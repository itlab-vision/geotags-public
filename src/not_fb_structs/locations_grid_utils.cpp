// Copyright 2025 itlab-vision

#include "locations_grid_utils.hpp"    // NOLINT
#include "locations_segment_info.hpp"  // NOLINT

#include <algorithm>
#include <cmath>
#include <climits>
#include <stdexcept>
#include <iostream>
#include <tuple>
#include <utility>
#include <limits>
#include <vector>

static void init_bbox(
    const std::vector<std::pair<LocationInfo, Coordinates>>& locations,
    double& lat_min, double& lat_max, double& lon_min, double& lon_max) {
    lat_min = locations[0].second.latitude;
    lat_max = locations[0].second.latitude;
    lon_min = locations[0].second.longitude;
    lon_max = locations[0].second.longitude;

    for (const auto& pt : locations) {
        lat_min = std::min(lat_min, pt.second.latitude);
        lat_max = std::max(lat_max, pt.second.latitude);
        lon_min = std::min(lon_min, pt.second.longitude);
        lon_max = std::max(lon_max, pt.second.longitude);
    }
}

//
// Function `compute_grid` and helpers
//

static std::vector<LocationsSegmentInfo> build_segments(int rows, int cols,
                                                        double lat_min,
                                                        double lon_min,
                                                        double cell_size) {
    std::vector<LocationsSegmentInfo> segments;
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

static void assign_points_to_segments(
    const std::vector<std::pair<LocationInfo, Coordinates>>& locations,
    std::vector<LocationsSegmentInfo>& segments, int rows, int cols,
    double lat_min, double lon_min, double cell_size) {
    for (const auto& pt : locations) {
        int r = std::min(
            static_cast<int>((pt.second.latitude - lat_min) / cell_size),
            rows - 1);

        int c = std::min(
            static_cast<int>((pt.second.longitude - lon_min) / cell_size),
            cols - 1);

        segments[r * cols + c].locations.push_back(pt.first.location_id);
    }
}

static void sort_segment_locations(
    std::vector<LocationsSegmentInfo>& segments,
    const std::vector<std::pair<LocationInfo, Coordinates>>& locations) {
    for (auto& segment : segments) {
        std::sort(segment.locations.begin(), segment.locations.end(),
                  [&](unsigned int a, unsigned int b) {
                      return locations[a].second.latitude <
                             locations[b].second.latitude;
                  });
    }
}

static void compute_neighbors(std::vector<LocationsSegmentInfo>& segments,
                              int rows, int cols) {
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

std::tuple<std::vector<LocationsSegmentInfo>, int, int> compute_grid(
    const std::vector<std::pair<LocationInfo, Coordinates>>& locations,
    double cell_size) {
    double lat_min, lat_max, lon_min, lon_max;
    init_bbox(locations, lat_min, lat_max, lon_min, lon_max);

    int rows = static_cast<int>(std::ceil((lat_max - lat_min) / cell_size));
    int cols = static_cast<int>(std::ceil((lon_max - lon_min) / cell_size));

    auto segments = build_segments(rows, cols, lat_min, lon_min, cell_size);
    assign_points_to_segments(locations, segments, rows, cols, lat_min, lon_min,
                              cell_size);
    sort_segment_locations(segments, locations);
    compute_neighbors(segments, rows, cols);

    return {segments, cols, rows};
}

//
// Function `count_points_in_grid` and helpers
//

static std::vector<int> count_points_in_grid(
    const std::vector<std::pair<LocationInfo, Coordinates>>& locations,
    int rows, int cols, double lat_min, double lon_min, double cell_size) {
    std::vector<int> counts(rows * cols, 0);

    for (const auto& pt : locations) {
        int r = std::min(
            static_cast<int>((pt.second.latitude - lat_min) / cell_size),
            rows - 1);

        int c = std::min(
            static_cast<int>((pt.second.longitude - lon_min) / cell_size),
            cols - 1);

        counts[r * cols + c]++;
    }

    return counts;
}

static std::tuple<double, int, int> compute_grid_stats(
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

static double compute_grid_score(double median, int maxc, int rows, int cols,
                                 int total_locations) {
    double penalty_max =
        (static_cast<double>(maxc) /
         std::max(1.0, std::sqrt(static_cast<double>(total_locations)))) *
        0.5;

    double size_penalty = static_cast<double>(rows * cols) * 0.001;

    return median + penalty_max + size_penalty;
}

double compute_optimal_cell_size(
    const std::vector<std::pair<LocationInfo, Coordinates>>& locations,
    int max_grid_size) {
    double lat_min, lat_max, lon_min, lon_max;
    init_bbox(locations, lat_min, lat_max, lon_min, lon_max);

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

        auto counts = count_points_in_grid(locations, rows, cols, lat_min,
                                           lon_min, cell_size);
        auto [median, maxc, minc] = compute_grid_stats(counts);

        double score =
            compute_grid_score(median, maxc, rows, cols, locations.size());
        if (score < best_score) {
            best_score = score;
            best_cell_size = cell_size;
        }

        curr_grid_size++;
    }

    return best_cell_size;
}
