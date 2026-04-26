// Copyright 2025 itlab-vision

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <utility>

#include "nearest_location.hpp"  // NOLINT
#include "coordinates.hpp"       // NOLINT
#include "location_info.hpp"     // NOLINT
#include "district_info.hpp"     // NOLINT
#include "distance.hpp"          // NOLINT
#include "nearest_location.hpp"  // NOLINT

extern "C" {
#include "tg.h"  // NOLINT
}

// Parameterized tests for NearestLocationGrid and NearestLocationGridBinary
class NearestLocationGridTest : public ::testing::TestWithParam<std::string> {
 protected:
    void SetUp() override {
        dist_calculator = Distance::create("haversine");
        if (!dist_calculator) {
            throw std::runtime_error("Failed to create distance calculator");
        }

        searcher = NearestLocation::create(GetParam());
        if (!searcher) {
            throw std::runtime_error(
                "Failed to create nearest location searcher for type: " +
                GetParam());
        }

        moscow = LocationInfo("Russia", "Moscow", {"Moskva"});
        tver = LocationInfo("Russia", "Tver", {"Tver City"});
        vladimir = LocationInfo("Russia", "Vladimir", {"Vladimir City"});
        spb = LocationInfo("Russia", "St. Petersburg", {"SPB", "Piter"});
        novgorod = LocationInfo("Russia", "Novgorod", {"Great Novgorod"});

        moscow_coords = Coordinates(55.75, 37.62);
        tver_coords = Coordinates(56.86, 35.20);
        vladimir_coords = Coordinates(56.14, 40.41);
        spb_coords = Coordinates(59.93, 30.36);
        novgorod_coords = Coordinates(58.52, 31.28);
    }

    std::vector<Pair<LocationInfo, Coordinates>> CreateLocationList(
        const std::vector<std::pair<LocationInfo, Coordinates>>& data) {
        std::vector<Pair<LocationInfo, Coordinates>> result;
        for (const auto& pair : data) {
            result.push_back(pair);
        }
        return result;
    }

    // Build a 3x3 grid covering the area of interest
    std::vector<LocationsSegmentInfo> BuildSegments(
        const std::vector<Pair<LocationInfo, Coordinates>>& locations) {
        const double min_lat = 55.0;
        const double max_lat = 61.0;
        const double min_lon = 29.0;
        const double max_lon = 41.0;
        const int rows = 3;
        const int cols = 3;
        const double lat_step = (max_lat - min_lat) / rows;
        const double lon_step = (max_lon - min_lon) / cols;

        std::vector<LocationsSegmentInfo> segments;
        segments.reserve(rows * cols);

        // First pass: create segments with empty location lists
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                unsigned int id = row * cols + col;
                double lat_min = min_lat + row * lat_step;
                double lat_max = min_lat + (row + 1) * lat_step;
                double lon_min = min_lon + col * lon_step;
                double lon_max = min_lon + (col + 1) * lon_step;

                // Slight overlap to avoid points exactly on boundaries
                // (though our points are well inside)
                segments.emplace_back(id, std::make_pair(lat_min, lat_max),
                                      std::make_pair(lon_min, lon_max),
                                      std::vector<unsigned int>(),
                                      std::vector<unsigned int>());
            }
        }

        // Assign locations to segments
        for (size_t idx = 0; idx < locations.size(); ++idx) {
            const auto& coord = locations[idx].second;
            int row = static_cast<int>((coord.latitude - min_lat) / lat_step);
            int col = static_cast<int>((coord.longitude - min_lon) / lon_step);
            // Clamp to valid range (should not happen)
            row = std::max(0, std::min(row, rows - 1));
            col = std::max(0, std::min(col, cols - 1));
            unsigned int seg_id = row * cols + col;
            segments[seg_id].locations.push_back(
                static_cast<unsigned int>(idx));
        }

        // Define neighbours (8-directional)
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                unsigned int id = row * cols + col;
                for (int dr = -1; dr <= 1; ++dr) {
                    for (int dc = -1; dc <= 1; ++dc) {
                        if (dr == 0 && dc == 0) continue;
                        int nr = row + dr;
                        int nc = col + dc;
                        if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
                            segments[id].neighbors.push_back(nr * cols + nc);
                        }
                    }
                }
            }
        }

        return segments;
    }

    Ptr<Distance> dist_calculator;
    Ptr<NearestLocation> searcher;
    LocationInfo moscow, tver, vladimir, spb, novgorod;
    Coordinates moscow_coords, tver_coords, vladimir_coords, spb_coords,
        novgorod_coords;
};

TEST_P(NearestLocationGridTest, Basic) {
    auto locations = CreateLocationList({{moscow, moscow_coords},
                                         {tver, tver_coords},
                                         {vladimir, vladimir_coords}});
    auto segments = BuildSegments(locations);

    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;

    auto state = searcher->calculate(locations, segments, dist_calculator,
                                     query, result, distance, 0.0);

    EXPECT_EQ(state, NearestLocationState::ExactLocation);
}

TEST_P(NearestLocationGridTest, Basic_City) {
    auto locations = CreateLocationList({{moscow, moscow_coords},
                                         {tver, tver_coords},
                                         {vladimir, vladimir_coords}});
    auto segments = BuildSegments(locations);
    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, segments, dist_calculator, query, result,
                        distance, 0.0);
    EXPECT_EQ(result.city, "Moscow");
}

TEST_P(NearestLocationGridTest, Basic_Country) {
    auto locations = CreateLocationList({{moscow, moscow_coords},
                                         {tver, tver_coords},
                                         {vladimir, vladimir_coords}});
    auto segments = BuildSegments(locations);
    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, segments, dist_calculator, query, result,
                        distance, 0.0);
    EXPECT_EQ(result.country, "Russia");
}

TEST_P(NearestLocationGridTest, Basic_DistancePositive) {
    auto locations = CreateLocationList({{moscow, moscow_coords},
                                         {tver, tver_coords},
                                         {vladimir, vladimir_coords}});
    auto segments = BuildSegments(locations);
    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, segments, dist_calculator, query, result,
                        distance, 0.0);
    EXPECT_GT(distance, 0.0);
}

TEST_P(NearestLocationGridTest, ExactMatch) {
    auto locations = CreateLocationList({{moscow, moscow_coords}});
    auto segments = BuildSegments(locations);

    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;

    auto state = searcher->calculate(locations, segments, dist_calculator,
                                     query, result, distance, 0.0);

    EXPECT_EQ(state, NearestLocationState::ExactLocation);
}

TEST_P(NearestLocationGridTest, ExactMatch_City) {
    auto locations = CreateLocationList({{moscow, moscow_coords}});
    auto segments = BuildSegments(locations);
    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, segments, dist_calculator, query, result,
                        distance, 0.0);
    EXPECT_EQ(result.city, "Moscow");
}

TEST_P(NearestLocationGridTest, ExactMatch_DistanceZero) {
    auto locations = CreateLocationList({{moscow, moscow_coords}});
    auto segments = BuildSegments(locations);
    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, segments, dist_calculator, query, result,
                        distance, 0.0);
    EXPECT_DOUBLE_EQ(distance, 0.0);
}

TEST_P(NearestLocationGridTest, MultipleLocations) {
    auto locations = CreateLocationList({{moscow, moscow_coords},
                                         {spb, spb_coords},
                                         {novgorod, novgorod_coords},
                                         {tver, tver_coords}});
    auto segments = BuildSegments(locations);

    Coordinates query(56.80, 35.15);
    LocationInfo result;
    double distance;

    auto state = searcher->calculate(locations, segments, dist_calculator,
                                     query, result, distance, 0.0);

    EXPECT_EQ(state, NearestLocationState::ExactLocation);
}

TEST_P(NearestLocationGridTest, MultipleLocations_City) {
    auto locations = CreateLocationList({{moscow, moscow_coords},
                                         {spb, spb_coords},
                                         {novgorod, novgorod_coords},
                                         {tver, tver_coords}});
    auto segments = BuildSegments(locations);
    Coordinates query(56.80, 35.15);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, segments, dist_calculator, query, result,
                        distance, 0.0);
    EXPECT_EQ(result.city, "Tver");
}

TEST_P(NearestLocationGridTest, MultipleLocations_DistancePositive) {
    auto locations = CreateLocationList({{moscow, moscow_coords},
                                         {spb, spb_coords},
                                         {novgorod, novgorod_coords},
                                         {tver, tver_coords}});
    auto segments = BuildSegments(locations);
    Coordinates query(56.80, 35.15);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, segments, dist_calculator, query, result,
                        distance, 0.0);
    EXPECT_GT(distance, 0.0);
}

TEST_P(NearestLocationGridTest, WithThreshold) {
    auto locations =
        CreateLocationList({{moscow, moscow_coords}, {tver, tver_coords}});
    auto segments = BuildSegments(locations);

    // This point lies in an empty segment (row2, col2) and will trigger
    // expansion
    Coordinates query(60.0, 40.0);
    LocationInfo result;
    double distance;

    auto state = searcher->calculate(locations, segments, dist_calculator,
                                     query, result, distance, 1.0);

    EXPECT_EQ(state, NearestLocationState::ApproximateLocation);
}

TEST_P(NearestLocationGridTest, WithThreshold_Distance) {
    auto locations =
        CreateLocationList({{moscow, moscow_coords}, {tver, tver_coords}});
    auto segments = BuildSegments(locations);
    Coordinates query(60.0, 40.0);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, segments, dist_calculator, query, result,
                        distance, 1.0);
    EXPECT_GT(distance, 1.0);
}

TEST_P(NearestLocationGridTest, ThresholdZero) {
    auto locations = CreateLocationList({{moscow, moscow_coords}});
    auto segments = BuildSegments(locations);

    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;

    auto state = searcher->calculate(locations, segments, dist_calculator,
                                     query, result, distance, 0.0);

    EXPECT_EQ(state, NearestLocationState::ExactLocation);
}

TEST_P(NearestLocationGridTest, ThresholdZero_DistanceZero) {
    auto locations = CreateLocationList({{moscow, moscow_coords}});
    auto segments = BuildSegments(locations);
    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, segments, dist_calculator, query, result,
                        distance, 0.0);
    EXPECT_DOUBLE_EQ(distance, 0.0);
}

TEST_P(NearestLocationGridTest, AlternativeNames) {
    auto locations =
        CreateLocationList({{moscow, moscow_coords}, {spb, spb_coords}});
    auto segments = BuildSegments(locations);

    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;

    auto state = searcher->calculate(locations, segments, dist_calculator,
                                     query, result, distance, 0.0);

    EXPECT_EQ(result.city, "Moscow");
}

TEST_P(NearestLocationGridTest, AlternativeNames_Size) {
    auto locations =
        CreateLocationList({{moscow, moscow_coords}, {spb, spb_coords}});
    auto segments = BuildSegments(locations);
    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, segments, dist_calculator, query, result,
                        distance, 0.0);
    EXPECT_EQ(result.alt_names.size(), 1);
}

TEST_P(NearestLocationGridTest, AlternativeNames_FirstAlt) {
    auto locations =
        CreateLocationList({{moscow, moscow_coords}, {spb, spb_coords}});
    auto segments = BuildSegments(locations);
    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(locations, segments, dist_calculator, query, result,
                        distance, 0.0);
    EXPECT_EQ(result.alt_names[0], "Moskva");
}

TEST_P(NearestLocationGridTest, EmptyList) {
    std::vector<Pair<LocationInfo, Coordinates>> empty_locations;
    auto segments = BuildSegments(empty_locations);  // empty segments

    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;

    EXPECT_THROW(
        {
            searcher->calculate(empty_locations, segments, dist_calculator,
                                query, result, distance, 0.0);
        },
        std::runtime_error);
}

// Instantiate tests for both grid-based implementations
INSTANTIATE_TEST_SUITE_P(GridImplementations, NearestLocationGridTest,
                         ::testing::Values("grid", "grid_binary"),
                         [](const ::testing::TestParamInfo<std::string>& info) {
                             return info.param;
                         });
