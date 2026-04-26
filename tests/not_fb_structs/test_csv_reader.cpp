// Copyright 2025 itlab-vision

#include <gtest/gtest.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "locations_reader.hpp"  // NOLINT

class CSVLocationsReaderTest : public ::testing::Test {
 protected:
    void SetUp() override {
        test_data_path = TEST_DATA_DIR;

        city_test_file =
            CreateTempFile("city_data",
                           "2\n"
                           "id;country;city;lat;lon\n"
                           "0;\"Russia\";\"Moscow\";55.7558;37.6173\n"
                           "1;\"Germany\";\"Berlin\";52.5200;13.4050\n");

        full_data_file =
            CreateTempFile("full_data",
                           "1\n"
                           "id;country;city;lat;lon;admin_id;alt_names\n"
                           "0;\"Russia\";\"Moscow\";55.7558;37.6173;\"39\";"
                           "\"Moskva\",\"Moskwa\"\n");

        empty_file = CreateTempFile("empty", "");

        only_headers_file =
            CreateTempFile("only_headers", "id;country;city;lat;lon\n");

        trailing_empty_file =
            CreateTempFile("trailing_empty",
                           "1\n"
                           "id;country,;city;lat;lon;admin_id;alt_names\n"
                           "0;\"Russia\";\"Moscow\";55.7558;37.6173;1;\n");

        with_carriage_returns_file =
            CreateTempFile("with_carriage_returns",
                           "1\n"
                           "id;country;city;lat;lon\n"
                           "0;\"Russia\"\r;\"Moscow\"\r;55.7558;37.6173\r\n");

        quotes_in_data_file =
            CreateTempFile("quotes_in_data",
                           "1\n"
                           "id;country;city;lat;lon\n"
                           "0;\"Ru\"ssia\";\"Mos\"cow\";55.7558;37.6173\n");

        extra_commas_file = CreateTempFile(
            "extra_commas",
            "1\n"
            "id;country;city;lat;lon\n"
            "0;\"Russia\";\"Moscow\";55.7558;37.6173;extra;data\n");

        empty_lines_file =
            CreateTempFile("empty_lines",
                           "1\n"
                           "id;country;city;lat;lon\n"
                           "\n"
                           "0;\"Russia\";\"Moscow\";55.7558;37.6173\n"
                           "\n");

        boundary_coordinates_file =
            CreateTempFile("boundary_coordinates",
                           "1\n"
                           "id;country;city;lat;lon\n"
                           "0;\"MinCoord\";\"Test1\";-90.0;-180.0\n"
                           "1;\"MaxCoord\";\"Test2\";90.0;180.0\n");

        different_number_formats_file =
            CreateTempFile("different_number_formats",
                           "1\n"
                           "id;country,city,lat,lon\n"
                           "0;\"Test1\";\"City1\";55;37\n"
                           "1;\"Test2\";\"City2\";55.7558;37.6173\n"
                           "2;\"Test3\";\"City3\";-45.1234;123.4567\n");
    }

    void TearDown() override {
        for (const auto& filename : test_files) {
            if (fileExists(filename)) {
                std::remove(filename.c_str());
            }
        }
        test_files.clear();
    }

    std::string CreateTempFile(const std::string& file_type,
                               const std::string& content) {
        static std::unordered_map<std::string, int> counters;
        int& counter = counters[file_type];

        std::string filename = test_data_path + "test_csv_" + file_type + "_" +
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

    std::string city_test_file;
    std::string full_data_file;
    std::string empty_file;
    std::string only_headers_file;
    std::string trailing_empty_file;
    std::string with_carriage_returns_file;
    std::string quotes_in_data_file;
    std::string extra_commas_file;
    std::string empty_lines_file;
    std::string boundary_coordinates_file;
    std::string different_number_formats_file;
};

TEST_F(CSVLocationsReaderTest, ValidFile_Constructor_DoesNotThrowException) {
    EXPECT_NO_THROW(CSVLocationsReader reader(city_test_file));
}

TEST_F(CSVLocationsReaderTest, NonexistentFile_Constructor_ThrowsException) {
    EXPECT_THROW(CSVLocationsReader reader("nonexistent_file.csv"),
                 std::invalid_argument);
}

TEST_F(CSVLocationsReaderTest, EmptyFile_Read_ReturnsEmptyLocations) {
    CSVLocationsReader reader(empty_file);
    auto locations = reader.read();
    EXPECT_TRUE(locations.empty());
}

TEST_F(CSVLocationsReaderTest, OnlyHeadersFile_Read_ReturnsEmptyLocations) {
    CSVLocationsReader reader(only_headers_file);
    auto locations = reader.read();
    EXPECT_TRUE(locations.empty());
}

TEST_F(CSVLocationsReaderTest, CityFile_Read_ReturnsCorrectNumberOfLocations) {
    CSVLocationsReader reader(city_test_file);
    auto locations = reader.read();
    EXPECT_EQ(locations.size(), 2);
}

TEST_F(CSVLocationsReaderTest, EmptyLinesFile_Read_ReturnsOnlyValidLines) {
    CSVLocationsReader reader(empty_lines_file);
    auto locations = reader.read();
    EXPECT_EQ(locations.size(), 1);
}

TEST_F(CSVLocationsReaderTest, FullDataFile_Location_HasCorrectCountry) {
    CSVLocationsReader reader(city_test_file);
    auto locations = reader.read();
    EXPECT_EQ(locations[0].first.country, "Russia");
}

TEST_F(CSVLocationsReaderTest, FullDataFile_Location_HasCorrectCity) {
    CSVLocationsReader reader(city_test_file);
    auto locations = reader.read();
    EXPECT_EQ(locations[0].first.city, "Moscow");
}

TEST_F(CSVLocationsReaderTest, FullDataFile_Location_HasCorrectLatitude) {
    CSVLocationsReader reader(city_test_file);
    auto locations = reader.read();
    EXPECT_DOUBLE_EQ(locations[0].second.latitude, 55.7558);
}

TEST_F(CSVLocationsReaderTest, FullDataFile_Location_HasCorrectLongitude) {
    CSVLocationsReader reader(city_test_file);
    auto locations = reader.read();
    EXPECT_DOUBLE_EQ(locations[0].second.longitude, 37.6173);
}

TEST_F(CSVLocationsReaderTest, FullDataFile_Location_HasCorrectRegion) {
    CSVLocationsReader reader(full_data_file);
    auto locations = reader.read();
    EXPECT_EQ(locations[0].first.region_id, 39);
}

TEST_F(CSVLocationsReaderTest, FullDataFile_Location_HasCorrectAltNames) {
    CSVLocationsReader reader(full_data_file);
    auto locations = reader.read();
    std::vector<std::string> expected_alt_names = {"Moskva", "Moskwa"};
    EXPECT_EQ(locations[0].first.alt_names, expected_alt_names);
}

TEST_F(CSVLocationsReaderTest,
       FileCarriageReturns_CountryField_HasNoCarriageReturns) {
    CSVLocationsReader reader(with_carriage_returns_file);
    auto locations = reader.read();
    EXPECT_EQ(locations[0].first.country.find('\r'), std::string::npos);
}

TEST_F(CSVLocationsReaderTest,
       FileCarriageReturns_CityField_HasNoCarriageReturns) {
    CSVLocationsReader reader(with_carriage_returns_file);
    auto locations = reader.read();
    EXPECT_EQ(locations[0].first.city.find('\r'), std::string::npos);
}

TEST_F(CSVLocationsReaderTest, FileQuotes_CountryField_HasNoQuotes) {
    CSVLocationsReader reader(quotes_in_data_file);
    auto locations = reader.read();
    EXPECT_EQ(locations[0].first.country.find('\"'), std::string::npos);
}

TEST_F(CSVLocationsReaderTest, FileQuotes_CityField_HasNoQuotes) {
    CSVLocationsReader reader(quotes_in_data_file);
    auto locations = reader.read();
    EXPECT_EQ(locations[0].first.city.find('\"'), std::string::npos);
}

TEST_F(CSVLocationsReaderTest, TrailingEmptyFile_AltNames_IsEmpty) {
    CSVLocationsReader reader(trailing_empty_file);
    auto locations = reader.read();
    EXPECT_TRUE(locations[0].first.alt_names.empty());
}

TEST_F(CSVLocationsReaderTest, ExtraCommasFile_Country_IsNotEmpty) {
    CSVLocationsReader reader(extra_commas_file);
    auto locations = reader.read();
    EXPECT_EQ(locations[0].first.country, "Russia");
}

TEST_F(CSVLocationsReaderTest, ExtraCommasFile_City_IsNotEmpty) {
    CSVLocationsReader reader(extra_commas_file);
    auto locations = reader.read();
    EXPECT_EQ(locations[0].first.city, "Moscow");
}

TEST_F(CSVLocationsReaderTest, ExtraCommasFile_RegionID_IsDefault) {
    CSVLocationsReader reader(extra_commas_file);
    auto locations = reader.read();
    EXPECT_EQ(locations[0].first.region_id, -1);
}

TEST_F(CSVLocationsReaderTest,
       BoundaryCoordinates_FirstLocation_HasMinLatitude) {
    CSVLocationsReader reader(boundary_coordinates_file);
    auto locations = reader.read();
    EXPECT_DOUBLE_EQ(locations[0].second.latitude, -90.0);
}

TEST_F(CSVLocationsReaderTest,
       BoundaryCoordinates_FirstLocation_HasMinLongitude) {
    CSVLocationsReader reader(boundary_coordinates_file);
    auto locations = reader.read();
    EXPECT_DOUBLE_EQ(locations[0].second.longitude, -180.0);
}

TEST_F(CSVLocationsReaderTest,
       BoundaryCoordinates_SecondLocation_HasMaxLatitude) {
    CSVLocationsReader reader(boundary_coordinates_file);
    auto locations = reader.read();
    EXPECT_DOUBLE_EQ(locations[1].second.latitude, 90.0);
}

TEST_F(CSVLocationsReaderTest,
       BoundaryCoordinates_SecondLocation_HasMaxLongitude) {
    CSVLocationsReader reader(boundary_coordinates_file);
    auto locations = reader.read();
    EXPECT_DOUBLE_EQ(locations[1].second.longitude, 180.0);
}
