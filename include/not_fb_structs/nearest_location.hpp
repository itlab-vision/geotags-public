// Copyright 2025 itlab-vision
#pragma once

#include <vector>
#include <string>
#include <utility>

#include "auxiliary.hpp"               // NOLINT
#include "location_info.hpp"           // NOLINT
#include "coordinates.hpp"             // NOLINT
#include "distance.hpp"                // NOLINT
#include "locations_segment_info.hpp"  // NOLINT

class NearestLocation {
 public:
    virtual NearestLocationState calculate(
        const std::vector<Pair<LocationInfo, Coordinates>> &locations,
        const std::vector<LocationsSegmentInfo> &segments,
        const Ptr<Distance> &dist_calculator, const Coordinates &location,
        LocationInfo &nearest_location, double &min_distance,
        const double threshold = 0.0) = 0;
    static Ptr<NearestLocation> create(const std::string &search_type);
};

class NearestLocationLinear : public NearestLocation {
 public:
    NearestLocationState calculate(
        const std::vector<Pair<LocationInfo, Coordinates>> &locations,
        const std::vector<LocationsSegmentInfo> &segments,
        const Ptr<Distance> &dist_calculator, const Coordinates &location,
        LocationInfo &nearest_location, double &min_distance,
        const double threshold = 0.0);
};

class NearestLocationGrid : public NearestLocation {
 protected:
    void update_bbox(const LocationsSegmentInfo &segment,
                     std::pair<double, double> &lon_range,
                     std::pair<double, double> &lat_range) const;
    int find_segment(const std::vector<LocationsSegmentInfo> &segments,
                     const Coordinates &location) const;
    void init_search_area(std::vector<unsigned int> &search_segments,
                          const std::vector<LocationsSegmentInfo> &segments,
                          const int &segment_id,
                          std::pair<double, double> &lon_range,
                          std::pair<double, double> &lat_range);
    void searchSegment(
        const LocationsSegmentInfo &segment,
        const std::vector<Pair<LocationInfo, Coordinates>> &locations,
        const Ptr<Distance> &dist_calculator, const Coordinates &loc_target,
        LocationInfo &nearest_location, double &min_distance, int &checked);
    void expand_neighbors(std::vector<uint32_t> &new_search_segments,
                          const std::vector<uint32_t> &search_segments,
                          const std::vector<LocationsSegmentInfo> &segments,
                          const std::pair<double, double> &lon_range,
                          const std::pair<double, double> &lat_range);

 public:
    NearestLocationState calculate(
        const std::vector<Pair<LocationInfo, Coordinates>> &locations,
        const std::vector<LocationsSegmentInfo> &segments,
        const Ptr<Distance> &dist_calculator, const Coordinates &location,
        LocationInfo &nearest_location, double &min_distance,
        const double threshold = 0.0);
};

class NearestLocationGridBinary : public NearestLocationGrid {
 private:
    void searchSegment(
        const LocationsSegmentInfo &segment,
        const std::vector<Pair<LocationInfo, Coordinates>> &locations,
        const Ptr<Distance> &dist_calculator, const Coordinates &loc_target,
        LocationInfo &nearest_location, double &min_distance, int &checked);
};
