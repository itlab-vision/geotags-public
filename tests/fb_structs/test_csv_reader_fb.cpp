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

class CSVLocationsReaderFBFlatBuffersTest : public ::testing::Test {
 protected:
    void SetUp() override {
        test_data_path = TEST_DATA_DIR;

        city_test_file =
            CreateTempFile("city_data_fb",
                           "2\n"
                           "id;country;city;lat;lon\n"
                           "0;\"Russia\";\"Moscow\";55.7558;37.6173\n"
                           "1;\"Germany\";\"Berlin\";52.5200;13.4050\n");

        full_data_file =
            CreateTempFile("full_data_fb",
                           "2\n"
                           "id;country;city;lat;lon;admin_id;alt_names\n"
                           "0;\"Russia\";\"Moscow\";55.7558;37.6173;\"39\";"
                           "1;\"Moskva\",\"Moskwa\"\n");

        malformed_file = CreateTempFile(
            "malformed_fb",
            "1\n"
            "id;country;city;lat;lon\n"
            "0;\"Russia\";\"Moscow\";invalid_lat46.3231;37.6173\n");

        empty_file = CreateTempFile("empty_fb", "");

        only_headers_file = CreateTempFile("only_headers_fb",
                                           "1\n"
                                           "id;country;city;lat;lon\n");

        trailing_empty_file =
            CreateTempFile("trailing_empty_fb",
                           "1\n"
                           "id;country,;city;lat;lon;admin_id;alt_names\n"
                           "0;\"Russia\";\"Moscow\";55.7558;37.6173;;\n");

        with_carriage_returns_file =
            CreateTempFile("with_carriage_returns_fb",
                           "1\n"
                           "id;country;city;lat;lon\n"
                           "0;\"Russia\"\r;\"Moscow\"\r;55.7558;37.6173\r\n");

        quotes_in_data_file =
            CreateTempFile("quotes_in_data_fb",
                           "1\n"
                           "id;country;city;lat;lon\n"
                           "0;\"Ru\"ssia\";\"Mos\"cow\";55.7558;37.6173\n");

        extra_commas_file = CreateTempFile(
            "extra_commas_fb",
            "1\n"
            "id;country;city;lat;lon\n"
            "0;\"Russia\";\"Moscow\";55.7558;37.6173;extra;data\n");

        empty_lines_file =
            CreateTempFile("empty_lines_fb",
                           "1\n"
                           "id;country;city;lat;lon\n"
                           "\n"
                           "0;\"Russia\";\"Moscow\";55.7558;37.6173\n"
                           "\n");

        boundary_coordinates_file =
            CreateTempFile("boundary_coordinates_fb",
                           "2\n"
                           "id;country;city;lat;lon\n"
                           "0;\"MinCoord\";\"Test1\";-90.0;-180.0\n"
                           "1;\"MaxCoord\";\"Test2\";90.0;180.0\n");

        different_number_formats_file =
            CreateTempFile("different_number_formats_fb",
                           "3\n"
                           "id;country;city;lat;lon\n"
                           "0;\"Test1\";\"City1\";55;37\n"
                           "1;\"Test2\";\"City2\";55.7558;37.6173\n"
                           "2;\"Test3\";\"City3\";-45.1234;123.4567\n");
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

        std::string filename = test_data_path + "test_csv_fb_" + file_type +
                               "_" + std::to_string(counter++) + ".csv";

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
    std::string malformed_file;
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

TEST_F(CSVLocationsReaderFBFlatBuffersTest,
       ValidFile_Constructor_DoesNotThrowException) {
    EXPECT_NO_THROW(CSVLocationsReaderFB reader(city_test_file));
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest,
       NonexistentFile_Constructor_ThrowsException) {
    EXPECT_THROW(CSVLocationsReaderFB reader("nonexistent_file.csv"),
                 std::invalid_argument);
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest, CityFile_Read_ReturnsValidPointer) {
    CSVLocationsReaderFB reader(city_test_file);
    auto locations = reader.read();
    EXPECT_NE(locations, nullptr);
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest,
       CityFile_Read_ReturnsCorrectNumberOfLocations) {
    CSVLocationsReaderFB reader(city_test_file);
    auto locations = reader.read();
    EXPECT_EQ(locations->size(), 2);
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest, EmptyFile_Read_ReturnsEmptyVector) {
    CSVLocationsReaderFB reader(empty_file);
    auto locations = reader.read();
    EXPECT_EQ(locations->size(), 0);
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest,
       OnlyHeadersFile_Read_ReturnsEmptyVector) {
    CSVLocationsReaderFB reader(only_headers_file);
    auto locations = reader.read();
    EXPECT_EQ(locations->size(), 0);
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest,
       EmptyLinesFile_Read_ReturnsOnlyValidLines) {
    CSVLocationsReaderFB reader(empty_lines_file);
    auto locations = reader.read();
    EXPECT_EQ(locations->size(), 1);
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest,
       FullDataFile_FirstLocation_HasCorrectCountry) {
    CSVLocationsReaderFB reader(city_test_file);
    auto locations = reader.read();
    EXPECT_STREQ(locations->Get(0)->info()->country()->c_str(), "Russia");
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest,
       FullDataFile_FirstLocation_HasCorrectCity) {
    CSVLocationsReaderFB reader(city_test_file);
    auto locations = reader.read();
    EXPECT_STREQ(locations->Get(0)->info()->city()->c_str(), "Moscow");
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest,
       FullDataFile_FirstLocation_HasCorrectLatitude) {
    CSVLocationsReaderFB reader(city_test_file);
    auto locations = reader.read();
    EXPECT_DOUBLE_EQ(locations->Get(0)->coords()->latitude(), 55.7558);
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest,
       FullDataFile_FirstLocation_HasCorrectLongitude) {
    CSVLocationsReaderFB reader(city_test_file);
    auto locations = reader.read();
    EXPECT_DOUBLE_EQ(locations->Get(0)->coords()->longitude(), 37.6173);
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest,
       BoundaryCoordinates_FirstLocation_HasMinLatitude) {
    CSVLocationsReaderFB reader(boundary_coordinates_file);
    auto locations = reader.read();
    EXPECT_DOUBLE_EQ(locations->Get(0)->coords()->latitude(), -90.0);
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest,
       BoundaryCoordinates_FirstLocation_HasMinLongitude) {
    CSVLocationsReaderFB reader(boundary_coordinates_file);
    auto locations = reader.read();
    EXPECT_DOUBLE_EQ(locations->Get(0)->coords()->longitude(), -180.0);
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest,
       BoundaryCoordinates_SecondLocation_HasMaxLatitude) {
    CSVLocationsReaderFB reader(boundary_coordinates_file);
    auto locations = reader.read();
    EXPECT_DOUBLE_EQ(locations->Get(1)->coords()->latitude(), 90.0);
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest,
       BoundaryCoordinates_SecondLocation_HasMaxLongitude) {
    CSVLocationsReaderFB reader(boundary_coordinates_file);
    auto locations = reader.read();
    EXPECT_DOUBLE_EQ(locations->Get(1)->coords()->longitude(), 180.0);
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest,
       DifferentNumberFormats_Read_ParsesSuccessfully) {
    CSVLocationsReaderFB reader(different_number_formats_file);
    auto locations = reader.read();
    EXPECT_EQ(locations->size(), 3);
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest, Read_CreatesFBFile) {
    CSVLocationsReaderFB reader(city_test_file);
    std::string fb_file = city_test_file + ".fb";
    if (fileExists(fb_file)) std::remove(fb_file.c_str());
    (void)reader.read();
    EXPECT_TRUE(fileExists(fb_file));
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest, SecondLocation_HasCorrectCity) {
    CSVLocationsReaderFB reader(city_test_file);
    auto locations = reader.read();
    EXPECT_STREQ(locations->Get(1)->info()->city()->c_str(), "Berlin");
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest, SecondLocation_HasCorrectCountry) {
    CSVLocationsReaderFB reader(city_test_file);
    auto locations = reader.read();
    EXPECT_STREQ(locations->Get(1)->info()->country()->c_str(), "Germany");
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest,
       CarriageReturns_AreRemovedFromCountry) {
    CSVLocationsReaderFB reader(with_carriage_returns_file);
    auto locations = reader.read();
    std::string country = locations->Get(0)->info()->country()->str();
    EXPECT_EQ(country.find('\r'), std::string::npos);
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest,
       CarriageReturns_AreRemovedFromCity) {
    CSVLocationsReaderFB reader(with_carriage_returns_file);
    auto locations = reader.read();
    std::string city = locations->Get(0)->info()->city()->str();
    EXPECT_EQ(city.find('\r'), std::string::npos);
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest, Quotes_AreRemovedFromCountry) {
    CSVLocationsReaderFB reader(quotes_in_data_file);
    auto locations = reader.read();
    std::string country = locations->Get(0)->info()->country()->str();
    EXPECT_EQ(country.find('\"'), std::string::npos);
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest, Quotes_AreRemovedFromCity) {
    CSVLocationsReaderFB reader(quotes_in_data_file);
    auto locations = reader.read();
    std::string city = locations->Get(0)->info()->city()->str();
    EXPECT_EQ(city.find('\"'), std::string::npos);
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest,
       ExtraCommas_FirstLocationHasRussia) {
    CSVLocationsReaderFB reader(extra_commas_file);
    auto locations = reader.read();
    EXPECT_STREQ(locations->Get(0)->info()->country()->c_str(), "Russia");
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest,
       ExtraCommas_FirstLocationHasMoscow) {
    CSVLocationsReaderFB reader(extra_commas_file);
    auto locations = reader.read();
    EXPECT_STREQ(locations->Get(0)->info()->city()->c_str(), "Moscow");
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest,
       FullDataFile_FullData_FirstLocationCountryCorrect) {
    CSVLocationsReaderFB reader(full_data_file);
    auto locations = reader.read();
    EXPECT_STREQ(locations->Get(0)->info()->country()->c_str(), "Russia");
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest,
       FullDataFile_FullData_FirstLocationCityCorrect) {
    CSVLocationsReaderFB reader(full_data_file);
    auto locations = reader.read();
    EXPECT_STREQ(locations->Get(0)->info()->city()->c_str(), "Moscow");
}

TEST_F(CSVLocationsReaderFBFlatBuffersTest,
       FullDataFile_FullData_CoordinatesCorrect) {
    CSVLocationsReaderFB reader(full_data_file);
    auto locations = reader.read();
    EXPECT_DOUBLE_EQ(locations->Get(0)->coords()->latitude(), 55.7558);
}
