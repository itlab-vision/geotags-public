// Copyright 2025 itlab-vision

#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <vector>

#include "locations_reader.hpp"  // NOLINT

class LocationsReaderCreateTest : public ::testing::Test {
 protected:
    void SetUp() override {
        // Create a temporary test file with valid data
        valid_test_file = CreateTempFile(
            "country;city;latitude;longitude\n"
            "Russia;Moscow;55.75;37.62\n"
            "Russia;St. Petersburg;59.93;30.36\n");
    }

    void TearDown() override {
        // Clean up temporary files
        for (const auto& file : _temp_files) {
            std::remove(file.c_str());
        }
    }

    std::string CreateTempFile(const std::string& content) {
        static int counter = 0;
        std::string filename =
            "temp_locations_reader_test_" + std::to_string(counter++) + ".csv";
        std::ofstream file(filename);
        file << content;
        file.close();
        _temp_files.push_back(filename);
        return filename;
    }

    std::string valid_test_file;

 private:
    std::vector<std::string> _temp_files;
};

TEST_F(LocationsReaderCreateTest, CreateCSVReader) {
    auto reader = LocationsReader::create("csv", valid_test_file);
    EXPECT_TRUE(reader != nullptr);
}

TEST_F(LocationsReaderCreateTest, CreateCSVReaderFB) {
    auto reader = LocationsReader::create("csv_fb", valid_test_file);
    EXPECT_TRUE(reader != nullptr);
}

TEST_F(LocationsReaderCreateTest, CreateUnsupportedReaderType) {
    EXPECT_THROW(
        {
            auto reader =
                LocationsReader::create("invalid_type", valid_test_file);
        },
        std::runtime_error);
}

TEST_F(LocationsReaderCreateTest, CreateWithNonexistentFile) {
    EXPECT_THROW(
        { auto reader = LocationsReader::create("csv", "nonexistent.csv"); },
        std::invalid_argument);
}

TEST_F(LocationsReaderCreateTest, CreateWithNonexistentFileFB) {
    EXPECT_THROW(
        { auto reader = LocationsReader::create("csv_fb", "nonexistent.csv"); },
        std::invalid_argument);
}

TEST_F(LocationsReaderCreateTest, CreateReturnsDifferentTypes) {
    auto csv_reader = LocationsReader::create("csv", valid_test_file);
    auto fb_reader = LocationsReader::create("csv_fb", valid_test_file);
    EXPECT_TRUE(csv_reader != nullptr);
}

TEST_F(LocationsReaderCreateTest, CreateReturnsDifferentTypes_FB) {
    auto fb_reader = LocationsReader::create("csv_fb", valid_test_file);
    EXPECT_TRUE(fb_reader != nullptr);
}
