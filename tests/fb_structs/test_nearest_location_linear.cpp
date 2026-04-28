// Copyright 2025 itlab-vision

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <unordered_map>
#include <utility>

#include "locations_reader.hpp"  // NOLINT
#include "nearest_location.hpp"  // NOLINT
#include "coordinates.hpp"       // NOLINT
#include "distance.hpp"          // NOLINT

extern "C" {
#include "tg.h"  // NOLINT
}

class NearestLocationLinearFBTest : public ::testing::Test {
 protected:
    void SetUp() override {
        dist_calculator = Distance::create("haversine");
        if (!dist_calculator) {
            throw std::runtime_error("Failed to create distance calculator");
        }

        searcher = NearestLocation::create("linear");
        if (!searcher) {
            throw std::runtime_error("Failed to create NearestLocationLinear");
        }

        test_data_path = TEST_DATA_DIR;

        // Basic locations file
        basic_file = CreateTempFile(
            "basic_fb",
            "3\n"
            "id;country;city;lat;lon;admin_id;alt_names\n"
            "1;\"Russia\";\"Moscow\";55.75;37.62;0;\"Moskva\"\n"
            "2;\"Russia\";\"Tver\";56.86;35.20;0;\"Tver City\"\n"
            "3;\"Russia\";\"Vladimir\";56.14;40.41;0;\"Vladimir City\"\n");

        // Extended locations file (more cities)
        extended_file = CreateTempFile(
            "extended_fb",
            "5\n"
            "id;country;city;lat;lon;admin_id;alt_names\n"
            "1;\"Russia\";\"Moscow\";55.75;37.62;0;\"Moskva\"\n"
            "2;\"Russia\";\"Tver\";56.86;35.20;0;\"Tver City\"\n"
            "3;\"Russia\";\"Vladimir\";56.14;40.41;0;\"Vladimir City\"\n"
            "4;\"Russia\";\"St. Petersburg\";59.93;30.36;0;\"SPB\",\"Piter\"\n"
            "5;\"Russia\";\"Novgorod\";58.52;31.28;0;\"Great Novgorod\"\n");

        // Single location file
        single_file = CreateTempFile(
            "single_fb",
            "1\n"
            "id;country;city;lat;lon;admin_id;alt_names\n"
            "1;\"Russia\";\"Moscow\";55.75;37.62;0;\"Moskva\"\n");

        // Empty file
        empty_file = CreateTempFile("empty_fb", "");

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

    std::string basic_file;
    std::string extended_file;
    std::string single_file;
    std::string empty_file;
};

TEST_F(NearestLocationLinearFBTest, Basic) {
    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;

    auto state = searcher->calculate(locations_basic, nullptr, dist_calculator,
                                     query, result, distance, 0.0);

    EXPECT_EQ(state, NearestLocationState::ExactLocation);
}

TEST_F(NearestLocationLinearFBTest, Basic_City) {
    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(locations_basic, nullptr, dist_calculator, query,
                        result, distance, 0.0);
    EXPECT_EQ(result.city, "Moscow");
}

TEST_F(NearestLocationLinearFBTest, Basic_Country) {
    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(locations_basic, nullptr, dist_calculator, query,
                        result, distance, 0.0);
    EXPECT_EQ(result.country, "Russia");
}

TEST_F(NearestLocationLinearFBTest, Basic_DistancePositive) {
    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(locations_basic, nullptr, dist_calculator, query,
                        result, distance, 0.0);
    EXPECT_GT(distance, 0.0);
}

TEST_F(NearestLocationLinearFBTest, ExactMatch) {
    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;

    auto state = searcher->calculate(locations_single, nullptr, dist_calculator,
                                     query, result, distance, 0.0);

    EXPECT_EQ(state, NearestLocationState::ExactLocation);
}

TEST_F(NearestLocationLinearFBTest, ExactMatch_City) {
    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;
    searcher->calculate(locations_single, nullptr, dist_calculator, query,
                        result, distance, 0.0);
    EXPECT_EQ(result.city, "Moscow");
}

TEST_F(NearestLocationLinearFBTest, ExactMatch_DistanceZero) {
    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;
    searcher->calculate(locations_single, nullptr, dist_calculator, query,
                        result, distance, 0.0);
    EXPECT_DOUBLE_EQ(distance, 0.0);
}

TEST_F(NearestLocationLinearFBTest, MultipleLocations) {
    Coordinates query(56.80, 35.15);
    LocationInfo result;
    double distance;

    auto state =
        searcher->calculate(locations_extended, nullptr, dist_calculator, query,
                            result, distance, 0.0);

    EXPECT_EQ(state, NearestLocationState::ExactLocation);
}

TEST_F(NearestLocationLinearFBTest, MultipleLocations_City) {
    Coordinates query(56.80, 35.15);
    LocationInfo result;
    double distance;
    searcher->calculate(locations_extended, nullptr, dist_calculator, query,
                        result, distance, 0.0);
    EXPECT_EQ(result.city, "Tver");
}

TEST_F(NearestLocationLinearFBTest, MultipleLocations_DistancePositive) {
    Coordinates query(56.80, 35.15);
    LocationInfo result;
    double distance;
    searcher->calculate(locations_extended, nullptr, dist_calculator, query,
                        result, distance, 0.0);
    EXPECT_GT(distance, 0.0);
}

TEST_F(NearestLocationLinearFBTest, WithThreshold) {
    Coordinates query(60.0, 40.0);
    LocationInfo result;
    double distance;

    auto state = searcher->calculate(locations_basic, nullptr, dist_calculator,
                                     query, result, distance, 1.0);

    EXPECT_EQ(state, NearestLocationState::ApproximateLocation);
}

TEST_F(NearestLocationLinearFBTest, WithThreshold_Distance) {
    Coordinates query(60.0, 40.0);
    LocationInfo result;
    double distance;
    searcher->calculate(locations_basic, nullptr, dist_calculator, query,
                        result, distance, 1.0);
    EXPECT_GT(distance, 1.0);
}

TEST_F(NearestLocationLinearFBTest, ThresholdZero) {
    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;

    auto state = searcher->calculate(locations_single, nullptr, dist_calculator,
                                     query, result, distance, 0.0);

    EXPECT_EQ(state, NearestLocationState::ExactLocation);
}

TEST_F(NearestLocationLinearFBTest, ThresholdZero_DistanceZero) {
    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;
    searcher->calculate(locations_single, nullptr, dist_calculator, query,
                        result, distance, 0.0);
    EXPECT_DOUBLE_EQ(distance, 0.0);
}

TEST_F(NearestLocationLinearFBTest, AlternativeNames_Size) {
    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(locations_basic, nullptr, dist_calculator, query,
                        result, distance, 0.0);
    EXPECT_EQ(result.alt_names.size(), 1);
}

TEST_F(NearestLocationLinearFBTest, AlternativeNames_FirstAlt) {
    Coordinates query(55.80, 37.70);
    LocationInfo result;
    double distance;
    searcher->calculate(locations_basic, nullptr, dist_calculator, query,
                        result, distance, 0.0);
    EXPECT_EQ(result.alt_names[0], "Moskva");
}

TEST_F(NearestLocationLinearFBTest, EmptyList) {
    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;

    EXPECT_THROW(
        {
            searcher->calculate(locations_empty, nullptr, dist_calculator,
                                query, result, distance, 0.0);
        },
        std::runtime_error);
}

TEST_F(NearestLocationLinearFBTest, NullptrList) {
    Coordinates query(55.75, 37.62);
    LocationInfo result;
    double distance;

    EXPECT_THROW(
        {
            searcher->calculate(nullptr, nullptr, dist_calculator, query,
                                result, distance, 0.0);
        },
        std::runtime_error);
}
