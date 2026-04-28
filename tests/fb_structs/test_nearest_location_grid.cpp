// Copyright 2025 itlab-vision

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <limits>
#include <utility>

#include "locations_reader.hpp"           // NOLINT
#include "locations_segments_reader.hpp"  // NOLINT
#include "nearest_location.hpp"           // NOLINT
#include "coordinates.hpp"                // NOLINT
#include "distance.hpp"                   // NOLINT
#include "flatbuffers/flatbuffers.h"      // NOLINT
#include "locations_generated.hpp"        // NOLINT

#include "locations_segments_generated.hpp"  // NOLINT

extern "C" {
#include "tg.h"  // NOLINT
}

// Helper to create a flatbuffers vector of segments from given locations
class SegmentBuilder {
 public:
    using LocationsVector =
        flatbuffers::Vector<flatbuffers::Offset<LocationsData::Location>>;
    using SegmentsVector = flatbuffers::Vector<
        flatbuffers::Offset<LocationsSegmentsData::LocationsSegmentInfo>>;

    // Build segments and store the builder internally
    void Build(const LocationsVector* locations) {
        builder_.Clear();

        // Grid covering test area (55-61 lat, 29-41 lon)
        const double min_lat = 55.0;
        const double max_lat = 61.0;
        const double min_lon = 29.0;
        const double max_lon = 41.0;
        const int rows = 3;
        const int cols = 3;
        const double lat_step = (max_lat - min_lat) / rows;
        const double lon_step = (max_lon - min_lon) / cols;

        std::vector<
            flatbuffers::Offset<LocationsSegmentsData::LocationsSegmentInfo>>
            segment_offsets;
        std::vector<std::vector<uint32_t>> segment_locations(rows * cols);
        std::vector<std::vector<uint32_t>> neighbor_ids(rows * cols);

        // Assign locations to segments
        if (locations) {
            for (uint32_t idx = 0; idx < locations->size(); ++idx) {
                auto loc = locations->Get(idx);
                auto coords = loc->coords();
                double lat = coords->latitude();
                double lon = coords->longitude();

                int row = static_cast<int>((lat - min_lat) / lat_step);
                int col = static_cast<int>((lon - min_lon) / lon_step);
                row = std::max(0, std::min(row, rows - 1));
                col = std::max(0, std::min(col, cols - 1));
                uint32_t seg_id = row * cols + col;
                segment_locations[seg_id].push_back(idx);
            }
        }

        // Build neighbor lists for each segment
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                uint32_t id = row * cols + col;
                for (int dr = -1; dr <= 1; ++dr) {
                    for (int dc = -1; dc <= 1; ++dc) {
                        if (dr == 0 && dc == 0) continue;
                        int nr = row + dr;
                        int nc = col + dc;
                        if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
                            neighbor_ids[id].push_back(nr * cols + nc);
                        }
                    }
                }
            }
        }

        // Create segment table for each segment
        for (uint32_t id = 0; id < rows * cols; ++id) {
            int row = id / cols;
            int col = id % cols;
            double lat_min = min_lat + row * lat_step;
            double lat_max = min_lat + (row + 1) * lat_step;
            double lon_min = min_lon + col * lon_step;
            double lon_max = min_lon + (col + 1) * lon_step;

            // Create range structures (plain structs, not created via builder)
            LocationsSegmentsData::LatitudeRange lat_range(lat_min, lat_max);
            LocationsSegmentsData::LongitudeRange lon_range(lon_min, lon_max);

            auto neighbors_vec = builder_.CreateVector(neighbor_ids[id]);
            auto locs_vec = builder_.CreateVector(segment_locations[id]);

            auto segment = LocationsSegmentsData::CreateLocationsSegmentInfo(
                builder_, id, &lat_range, &lon_range, neighbors_vec, locs_vec);
            segment_offsets.push_back(segment);
        }

        auto segments_vec = builder_.CreateVector(segment_offsets);
        builder_.Finish(segments_vec);
    }

    const SegmentsVector* GetSegments() const {
        return flatbuffers::GetRoot<SegmentsVector>(
            builder_.GetBufferPointer());
    }

 private:
    flatbuffers::FlatBufferBuilder builder_;
};

// Parameterized tests for NearestLocationGrid and NearestLocationGridBinary
// with flatbuffers
class NearestLocationGridFBTest : public ::testing::TestWithParam<std::string> {
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

        test_data_path = TEST_DATA_DIR;

        // Create test CSV files with locations (same as linear tests)
        basic_file = CreateTempFile(
            "grid_basic_fb",
            "3\n"
            "id;country;city;lat;lon;admin_id;alt_names\n"
            "1;\"Russia\";\"Moscow\";55.75;37.62;0;\"Moskva\"\n"
            "2;\"Russia\";\"Tver\";56.86;35.20;0;\"Tver City\"\n"
            "3;\"Russia\";\"Vladimir\";56.14;40.41;0;\"Vladimir City\"\n");

        extended_file = CreateTempFile(
            "grid_extended_fb",
            "5\n"
            "id;country;city;lat;lon;admin_id;alt_names\n"
            "1;\"Russia\";\"Moscow\";55.75;37.62;0;\"Moskva\"\n"
            "2;\"Russia\";\"Tver\";56.86;35.20;0;\"Tver City\"\n"
            "3;\"Russia\";\"Vladimir\";56.14;40.41;0;\"Vladimir City\"\n"
            "4;\"Russia\";\"St. Petersburg\";59.93;30.36;0;\"SPB\",\"Piter\"\n"
            "5;\"Russia\";\"Novgorod\";58.52;31.28;0;\"Great Novgorod\"\n");

        single_file = CreateTempFile(
            "grid_single_fb",
            "1\n"
            "id;country;city;lat;lon;admin_id;alt_names\n"
            "1;\"Russia\";\"Moscow\";55.75;37.62;0;\"Moskva\"\n");

        empty_file = CreateTempFile("grid_empty_fb", "");

        // Read locations from CSV using readers (store readers to keep data
        // alive)
        auto reader_basic = std::make_unique<CSVLocationsReaderFB>(basic_file);
        locations_basic = reader_basic->read();
        location_readers.push_back(std::move(reader_basic));

        auto reader_extended =
            std::make_unique<CSVLocationsReaderFB>(extended_file);
        locations_extended = reader_extended->read();
        location_readers.push_back(std::move(reader_extended));

        auto reader_single =
            std::make_unique<CSVLocationsReaderFB>(single_file);
        locations_single = reader_single->read();
        location_readers.push_back(std::move(reader_single));

        auto reader_empty = std::make_unique<CSVLocationsReaderFB>(empty_file);
        locations_empty = reader_empty->read();
        location_readers.push_back(std::move(reader_empty));

        // Build segments for each locations set
        seg_builder_basic.Build(locations_basic);
        seg_builder_extended.Build(locations_extended);
        seg_builder_single.Build(locations_single);
        seg_builder_empty.Build(locations_empty);

        // Obtain pointers
        segments_basic = seg_builder_basic.GetSegments();
        segments_extended = seg_builder_extended.GetSegments();
        segments_single = seg_builder_single.GetSegments();
        segments_empty = seg_builder_empty.GetSegments();
    }

    void TearDown() override {
        for (const auto& filename : test_files) {
            if (fileExists(filename)) {
                std::remove(filename.c_str());
            }
            std::string fb = filename + ".fb";
            if (fileExists(fb)) std::remove(fb.c_str());
        }
        test_files.clear();
    }

    std::string CreateTempFile(const std::string& file_type,
                               const std::string& content) {
        static std::unordered_map<std::string, int> counters;
        int& counter = counters[file_type];

        std::string filename = test_data_path + "test_fb_" + file_type + "_" +
                               std::to_string(counter++) + ".csv";

        std::ofstream file(filename);
        if (!file) {
            throw std::runtime_error("Cannot create temp file: " + filename);
        }
        file << content;
        file.close();

        test_files.push_back(filename);
        return filename;
    }

    bool fileExists(const std::string& filename) {
        std::ifstream file(filename);
        return file.good();
    }

    std::vector<std::string> test_files;
    std::string test_data_path;

    Ptr<Distance> dist_calculator;
    Ptr<NearestLocation> searcher;

    // Locations (pointers into readers' data)
    const flatbuffers::Vector<flatbuffers::Offset<LocationsData::Location>>*
        locations_basic = nullptr;
    const flatbuffers::Vector<flatbuffers::Offset<LocationsData::Location>>*
        locations_extended = nullptr;
    const flatbuffers::Vector<flatbuffers::Offset<LocationsData::Location>>*
        locations_single = nullptr;
    const flatbuffers::Vector<flatbuffers::Offset<LocationsData::Location>>*
        locations_empty = nullptr;

    // Keep readers alive to preserve location data
    std::vector<std::unique_ptr<CSVLocationsReaderFB>> location_readers;

    // Segment builders (keep them alive for segment data)
    SegmentBuilder seg_builder_basic;
    SegmentBuilder seg_builder_extended;
    SegmentBuilder seg_builder_single;
    SegmentBuilder seg_builder_empty;

    // Pointers to segments vectors
    const flatbuffers::Vector<
        flatbuffers::Offset<LocationsSegmentsData::LocationsSegmentInfo>>*
        segments_basic = nullptr;
    const flatbuffers::Vector<
        flatbuffers::Offset<LocationsSegmentsData::LocationsSegmentInfo>>*
        segments_extended = nullptr;
    const flatbuffers::Vector<
        flatbuffers::Offset<LocationsSegmentsData::LocationsSegmentInfo>>*
        segments_single = nullptr;
    const flatbuffers::Vector<
        flatbuffers::Offset<LocationsSegmentsData::LocationsSegmentInfo>>*
        segments_empty = nullptr;

    std::string basic_file;
    std::string extended_file;
    std::string single_file;
    std::string empty_file;
};

TEST_P(NearestLocationGridFBTest, Basic) {
    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;

    auto state =
        searcher->calculate(locations_basic, segments_basic, dist_calculator,
                            query, result, distance, 0.0);

    EXPECT_EQ(state, NearestLocationState::ExactLocation);
}

TEST_P(NearestLocationGridFBTest, Basic_City) {
    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(locations_basic, segments_basic, dist_calculator, query,
                        result, distance, 0.0);
    EXPECT_EQ(result.city, "Moscow");
}

TEST_P(NearestLocationGridFBTest, Basic_Country) {
    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(locations_basic, segments_basic, dist_calculator, query,
                        result, distance, 0.0);
    EXPECT_EQ(result.country, "Russia");
}

TEST_P(NearestLocationGridFBTest, Basic_DistancePositive) {
    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(locations_basic, segments_basic, dist_calculator, query,
                        result, distance, 0.0);
    EXPECT_GT(distance, 0.0);
}

TEST_P(NearestLocationGridFBTest, ExactMatch) {
    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;

    auto state =
        searcher->calculate(locations_single, segments_single, dist_calculator,
                            query, result, distance, 0.0);

    EXPECT_EQ(state, NearestLocationState::ExactLocation);
}

TEST_P(NearestLocationGridFBTest, ExactMatch_City) {
    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;
    searcher->calculate(locations_single, segments_single, dist_calculator,
                        query, result, distance, 0.0);
    EXPECT_EQ(result.city, "Moscow");
}

TEST_P(NearestLocationGridFBTest, ExactMatch_DistanceZero) {
    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;
    searcher->calculate(locations_single, segments_single, dist_calculator,
                        query, result, distance, 0.0);
    EXPECT_DOUBLE_EQ(distance, 0.0);
}

TEST_P(NearestLocationGridFBTest, MultipleLocations) {
    Coordinates query(56.80, 35.15);
    LocationInfo result;
    double distance;

    auto state =
        searcher->calculate(locations_extended, segments_extended,
                            dist_calculator, query, result, distance, 0.0);

    EXPECT_EQ(state, NearestLocationState::ExactLocation);
}

TEST_P(NearestLocationGridFBTest, MultipleLocations_City) {
    Coordinates query(56.80, 35.15);
    LocationInfo result;
    double distance;
    searcher->calculate(locations_extended, segments_extended, dist_calculator,
                        query, result, distance, 0.0);
    EXPECT_EQ(result.city, "Tver");
}

TEST_P(NearestLocationGridFBTest, MultipleLocations_DistancePositive) {
    Coordinates query(56.80, 35.15);
    LocationInfo result;
    double distance;
    searcher->calculate(locations_extended, segments_extended, dist_calculator,
                        query, result, distance, 0.0);
    EXPECT_GT(distance, 0.0);
}

TEST_P(NearestLocationGridFBTest, WithThreshold) {
    Coordinates query(60.0, 40.0);
    LocationInfo result;
    double distance;

    auto state =
        searcher->calculate(locations_basic, segments_basic, dist_calculator,
                            query, result, distance, 1.0);

    EXPECT_EQ(state, NearestLocationState::ApproximateLocation);
}

TEST_P(NearestLocationGridFBTest, WithThreshold_Distance) {
    Coordinates query(60.0, 40.0);
    LocationInfo result;
    double distance;
    searcher->calculate(locations_basic, segments_basic, dist_calculator, query,
                        result, distance, 1.0);
    EXPECT_GT(distance, 1.0);
}

TEST_P(NearestLocationGridFBTest, ThresholdZero) {
    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;

    auto state =
        searcher->calculate(locations_single, segments_single, dist_calculator,
                            query, result, distance, 0.0);

    EXPECT_EQ(state, NearestLocationState::ExactLocation);
}

TEST_P(NearestLocationGridFBTest, ThresholdZero_DistanceZero) {
    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;
    searcher->calculate(locations_single, segments_single, dist_calculator,
                        query, result, distance, 0.0);
    EXPECT_DOUBLE_EQ(distance, 0.0);
}

TEST_P(NearestLocationGridFBTest, AlternativeNames_Size) {
    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(locations_basic, segments_basic, dist_calculator, query,
                        result, distance, 0.0);
    EXPECT_EQ(result.alt_names.size(), 1);
}

TEST_P(NearestLocationGridFBTest, AlternativeNames_FirstAlt) {
    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(locations_basic, segments_basic, dist_calculator, query,
                        result, distance, 0.0);
    EXPECT_EQ(result.alt_names[0], "Moskva");
}

TEST_P(NearestLocationGridFBTest, EmptyList) {
    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;

    EXPECT_THROW(
        {
            searcher->calculate(locations_empty, segments_empty,
                                dist_calculator, query, result, distance, 0.0);
        },
        std::runtime_error);
}

TEST_P(NearestLocationGridFBTest, NullptrList) {
    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;

    EXPECT_THROW(
        {
            searcher->calculate(nullptr, segments_basic, dist_calculator, query,
                                result, distance, 0.0);
        },
        std::runtime_error);
}

TEST_P(NearestLocationGridFBTest, NullptrSegments) {
    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;

    EXPECT_THROW(
        {
            searcher->calculate(locations_basic, nullptr, dist_calculator,
                                query, result, distance, 0.0);
        },
        std::runtime_error);
}

// Instantiate tests for both grid-based implementations
INSTANTIATE_TEST_SUITE_P(GridImplementations, NearestLocationGridFBTest,
                         ::testing::Values("grid", "grid_binary"),
                         [](const ::testing::TestParamInfo<std::string>& info) {
                             return info.param;
                         });
