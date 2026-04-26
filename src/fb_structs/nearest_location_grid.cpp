// Copyright 2025 itlab-vision

#include <limits>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>

#include "distance.hpp"          // NOLINT
#include "coordinates.hpp"       // NOLINT
#include "nearest_location.hpp"  // NOLINT

void NearestLocationGrid::update_bbox(
    const LocationsSegmentsData::LocationsSegmentInfo* segment,
    std::pair<double, double>& lon_range,
    std::pair<double, double>& lat_range) const {
    lon_range.first = std::min(lon_range.first, segment->lon_range()->min());
    lon_range.second = std::max(lon_range.second, segment->lon_range()->max());
    lat_range.first = std::min(lat_range.first, segment->lat_range()->min());
    lat_range.second = std::max(lat_range.second, segment->lat_range()->max());
}

int NearestLocationGrid::find_segment(
    const flatbuffers::Vector<flatbuffers::Offset<
        LocationsSegmentsData::LocationsSegmentInfo>>* segments,
    const Coordinates& location) const {
    // Compute row size
    int row_size = 0;
    const double first_lat = segments->Get(0)->lat_range()->min();
    for (size_t i = 0; i < segments->size(); i++) {
        const auto& seg = segments->Get(i);
        if (seg->lat_range()->min() != first_lat) {
            break;
        }
        row_size++;
    }

    int segment_id = -1;

    // find column
    if (location.longitude >
        segments->Get(segments->size() - 1)->lon_range()->min()) {
        segment_id = segments->size() - 1;
    } else if (location.longitude < segments->Get(0)->lon_range()->max()) {
        segment_id = 0;
    } else {
        segment_id = 0;
        while (location.longitude >=
               segments->Get(segment_id)->lon_range()->max()) {
            segment_id++;
        }
    }

    // adjust row
    if (location.latitude < segments->Get(segment_id)->lat_range()->min()) {
        while (segment_id - row_size >= 0 &&
               segments->Get(segment_id)->lat_range()->min() >
                   location.latitude) {
            segment_id -= row_size;
        }
    } else {
        while (segment_id + row_size < segments->size() &&
               segments->Get(segment_id)->lat_range()->max() <
                   location.latitude) {
            segment_id += row_size;
        }
    }

    return segment_id;
}

void NearestLocationGrid::init_search_area(
    std::vector<unsigned int>& search_segments,
    const flatbuffers::Vector<
        flatbuffers::Offset<LocationsSegmentsData::LocationsSegmentInfo>>*
        segments,
    const int& segment_id, std::pair<double, double>& lon_range,
    std::pair<double, double>& lat_range) {
    auto neighbors = segments->Get(segment_id)->neighbors();
    search_segments.reserve(neighbors->size() + 1);
    for (auto n : *neighbors) {
        search_segments.push_back(n);
    }
    search_segments.push_back(segment_id);

    lon_range.first = std::numeric_limits<double>::max();
    lon_range.second = std::numeric_limits<double>::lowest();
    lat_range.first = std::numeric_limits<double>::max();
    lat_range.second = std::numeric_limits<double>::lowest();
    for (auto seg_id : search_segments) {
        update_bbox(segments->Get(seg_id), lon_range, lat_range);
    }
}

void NearestLocationGrid::searchSegment(
    const LocationsSegmentsData::LocationsSegmentInfo* segment,
    const flatbuffers::Vector<flatbuffers::Offset<LocationsData::Location>>*
        locations,
    const Ptr<Distance>& dist_calculator, const Coordinates& loc_target,
    int& nearest_location_id, double& min_distance, int& checked) {
    auto loc_ids_fb = segment->locations();
    if (!loc_ids_fb || loc_ids_fb->size() == 0) return;

    for (auto loc_id : *loc_ids_fb) {
        double distance = dist_calculator->calculate(
            locations->Get(loc_id)->coords(), loc_target);
        if (distance < min_distance) {
            min_distance = distance;
            nearest_location_id = loc_id;
        }
        checked++;
    }
}

void NearestLocationGrid::expand_neighbors(
    std::vector<uint32_t>& new_search_segments,
    const std::vector<uint32_t>& search_segments,
    const flatbuffers::Vector<
        flatbuffers::Offset<LocationsSegmentsData::LocationsSegmentInfo>>*
        segments,
    const std::pair<double, double>& lon_range,
    const std::pair<double, double>& lat_range) {
    for (auto seg_id : search_segments) {
        auto neighbors_fb = segments->Get(seg_id)->neighbors();
        for (auto neighbor_id : *neighbors_fb) {
            auto neighbor = segments->Get(neighbor_id);

            if ((lon_range.second <= neighbor->lon_range()->min() ||
                 lon_range.first >= neighbor->lon_range()->max() ||
                 lat_range.second <= neighbor->lat_range()->min() ||
                 lat_range.first >= neighbor->lat_range()->max()) &&
                std::find(new_search_segments.begin(),
                          new_search_segments.end(),
                          neighbor_id) == new_search_segments.end()) {
                new_search_segments.push_back(neighbor_id);
            }
        }
    }
}

NearestLocationState NearestLocationGrid::calculate(
    const flatbuffers::Vector<flatbuffers::Offset<LocationsData::Location>>*
        locations,
    const flatbuffers::Vector<
        flatbuffers::Offset<LocationsSegmentsData::LocationsSegmentInfo>>*
        segments,
    const Ptr<Distance>& dist_calculator, const Coordinates& location,
    LocationInfo& nearest_location, double& min_distance,
    const double threshold) {
    if (!locations || locations->size() == 0) {
        throw std::runtime_error("List of available locations is empty");
    }
    if (!segments || segments->size() == 0) {
        throw std::runtime_error("Grid segments are not available");
    }

    // Find segment containing the location
    int segment_id = find_segment(segments, location);
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
    int nearest_location_id = -1;

    while (true) {
        for (auto seg_id : search_segments) {
            searchSegment(segments->Get(seg_id), locations, dist_calculator,
                          location, nearest_location_id, min_distance, checked);
        }

        if (checked != 0) break;

        std::vector<uint32_t> new_search_segments;
        expand_neighbors(new_search_segments, search_segments, segments,
                         lon_range, lat_range);

        if (new_search_segments.empty()) break;
        search_segments = new_search_segments;

        for (unsigned int seg_id : search_segments) {
            update_bbox(segments->Get(seg_id), lon_range, lat_range);
        }
    }

    if (checked == 0 || nearest_location_id < 0) {
        throw std::runtime_error(
            "Failed to find any location in the search segments");
    }

    convert_location_info(nearest_location,
                          locations->Get(nearest_location_id)->info());

    // Check threshold
    if (threshold > 0 && min_distance > threshold) {
        return NearestLocationState::ApproximateLocation;
    }
    return NearestLocationState::ExactLocation;
}
