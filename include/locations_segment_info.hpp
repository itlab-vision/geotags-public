// Copyright 2025 itlab-vision
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <utility>

struct LocationsSegmentInfo {
    unsigned int segment_id;
    std::pair<double, double> lat_range;  // (min, max)
    std::pair<double, double> lon_range;  // (min, max)
    std::vector<unsigned int> neighbors;  // neighboring segment ids
    std::vector<unsigned int> locations;  // cities in this segment

    LocationsSegmentInfo() {}
    LocationsSegmentInfo(unsigned int id, std::pair<double, double> lat_rng,
                         std::pair<double, double> lon_rng,
                         const std::vector<unsigned int> &nbrs,
                         const std::vector<unsigned int> &locs)
        : segment_id(id),
          lat_range(lat_rng),
          lon_range(lon_rng),
          neighbors(nbrs),
          locations(locs) {}

    friend std::ostream &operator<<(std::ostream &out,
                                    const LocationsSegmentInfo &info) {
        out << "Segment id: " << info.segment_id << "\n";
        out << "Latitude range: [" << info.lat_range.first << ", "
            << info.lat_range.second << "]\n";
        out << "Longitude range: [" << info.lon_range.first << ", "
            << info.lon_range.second << "]\n";
        return out;
    }
};
