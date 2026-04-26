// Copyright 2025 itlab-vision

#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <utility>

#include "distance.hpp"          // NOLINT
#include "coordinates.hpp"       // NOLINT
#include "nearest_location.hpp"  // NOLINT

void NearestLocationGrid::update_bbox(
    const LocationsSegmentInfo& segment, std::pair<double, double>& lon_range,
    std::pair<double, double>& lat_range) const {
    lon_range.first = std::min(lon_range.first, segment.lon_range.first);
    lon_range.second = std::max(lon_range.second, segment.lon_range.second);
    lat_range.first = std::min(lat_range.first, segment.lat_range.first);
    lat_range.second = std::max(lat_range.second, segment.lat_range.second);
}

int NearestLocationGrid::find_segment(
    const std::vector<LocationsSegmentInfo>& segments,
    const Coordinates& location) const {
    // Compute row size
    int row_size = 0;
    const double first_lat = segments.front().lat_range.first;
    for (const auto& seg : segments) {
        if (seg.lat_range.first != first_lat) {
            break;
        }
        row_size++;
    }

    int segment_id = -1;

    // find column
    if (location.longitude > segments.back().lon_range.first) {
        segment_id = segments.back().segment_id;
    } else if (location.longitude < segments.front().lon_range.second) {
        segment_id = 0;
    } else {
        segment_id = 0;
        while (location.longitude >= segments[segment_id].lon_range.second) {
            segment_id++;
        }
    }

    // adjust row
    if (location.latitude < segments[segment_id].lat_range.first) {
        while (segment_id - row_size >= 0 &&
               segments[segment_id].lat_range.first > location.latitude) {
            segment_id -= row_size;
        }
    } else {
        while (segment_id + row_size < segments.size() &&
               segments[segment_id].lat_range.second < location.latitude) {
            segment_id += row_size;
        }
    }

    return segment_id;
}

void NearestLocationGrid::init_search_area(
    std::vector<unsigned int>& search_segments,
    const std::vector<LocationsSegmentInfo>& segments, const int& segment_id,
    std::pair<double, double>& lon_range,
    std::pair<double, double>& lat_range) {
    search_segments = segments[segment_id].neighbors;
    search_segments.push_back(segment_id);

    lon_range.first = std::numeric_limits<double>::max();
    lon_range.second = std::numeric_limits<double>::lowest();
    lat_range.first = std::numeric_limits<double>::max();
    lat_range.second = std::numeric_limits<double>::lowest();
    for (auto seg_id : search_segments) {
        update_bbox(segments[seg_id], lon_range, lat_range);
    }
}

void NearestLocationGrid::searchSegment(
    const LocationsSegmentInfo& segment,
    const std::vector<Pair<LocationInfo, Coordinates>>& locations,
    const Ptr<Distance>& dist_calculator, const Coordinates& loc_target,
    LocationInfo& nearest_location, double& min_distance, int& checked) {
    for (unsigned int loc_id : segment.locations) {
        double distance =
            dist_calculator->calculate(locations[loc_id].second, loc_target);
        if (distance < min_distance) {
            nearest_location = locations[loc_id].first;
            min_distance = distance;
        }
        checked++;
    }
}

void NearestLocationGrid::expand_neighbors(
    std::vector<uint32_t>& new_search_segments,
    const std::vector<uint32_t>& search_segments,
    const std::vector<LocationsSegmentInfo>& segments,
    const std::pair<double, double>& lon_range,
    const std::pair<double, double>& lat_range) {
    for (unsigned int seg_id : search_segments) {
        for (unsigned int neighbor_id : segments[seg_id].neighbors) {
            const auto& neighbor = segments[neighbor_id];

            if ((lon_range.second <= neighbor.lon_range.first ||
                 lon_range.first >= neighbor.lon_range.second ||
                 lat_range.second <= neighbor.lat_range.first ||
                 lat_range.first >= neighbor.lat_range.second) &&
                std::find(new_search_segments.begin(),
                          new_search_segments.end(),
                          neighbor_id) == new_search_segments.end()) {
                new_search_segments.push_back(neighbor_id);
            }
        }
    }
}

NearestLocationState NearestLocationGrid::calculate(
    const std::vector<Pair<LocationInfo, Coordinates>>& locations,
    const std::vector<LocationsSegmentInfo>& segments,
    const Ptr<Distance>& dist_calculator, const Coordinates& location,
    LocationInfo& nearest_location, double& min_distance,
    const double threshold) {
    if (locations.size() == 0) {
        throw std::runtime_error("List of available locations is empty");
    }
    if (segments.size() == 0) {
        throw std::runtime_error("Grid segments are not available");
    }

    // Find segment containing the location
    int segment_id = find_segment(segments, location);
    if (segment_id < 0 || segment_id >= segments.size()) {
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

    while (true) {
        for (unsigned int seg_id : search_segments) {
            searchSegment(segments[seg_id], locations, dist_calculator,
                          location, nearest_location, min_distance, checked);
        }

        if (checked != 0) break;

        std::vector<uint32_t> new_search_segments;
        expand_neighbors(new_search_segments, search_segments, segments,
                         lon_range, lat_range);

        if (new_search_segments.empty()) break;
        search_segments = new_search_segments;

        for (unsigned int seg_id : search_segments) {
            update_bbox(segments[seg_id], lon_range, lat_range);
        }
    }

    if (checked == 0) {
        throw std::runtime_error(
            "Failed to find any location in the search segments");
    }

    // Check threshold
    if (threshold > 0 && min_distance > threshold) {
        return NearestLocationState::ApproximateLocation;
    }
    return NearestLocationState::ExactLocation;
}
