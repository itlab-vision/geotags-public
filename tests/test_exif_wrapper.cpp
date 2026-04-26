// Copyright 2025 itlab-vision

#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <cstdio>
#include <regex>

#include "exif_wrapper.hpp"  // NOLINT

class EXIFDataTest : public ::testing::Test {
 protected:
    void SetUp() override {
        test_images_path = TEST_DATA_DIR;

        image_with_gps_and_datetime =
            test_images_path + "image_with_gps_datatime.jpg";
        image_without_datetime =
            test_images_path + "image_without_datetime.jpg";
        image_without_gps = test_images_path + "tmp_image_without_gps.jpg";
        image_corrupted = test_images_path + "tmp_corrupted.jpg";
        image_nonexistent = test_images_path + "tmp_nonexistent.jpg";

        checkTestFiles();
        createTestImages();
    }

    void TearDown() override {
        if (fileExists(image_corrupted)) {
            std::remove(image_corrupted.c_str());
        }
        if (fileExists(image_without_gps)) {
            std::remove(image_without_gps.c_str());
        }
    }

    void checkTestFiles() {
        if (!fileExists(image_with_gps_and_datetime)) {
            GTEST_SKIP() << "Test image with GPS and datetime not found: "
                         << image_with_gps_and_datetime;
        }
        if (!fileExists(image_without_datetime)) {
            GTEST_SKIP() << "Test image without datetime not found: "
                         << image_without_datetime;
        }
    }

    void createTestImages() {
        if (!fileExists(image_corrupted)) {
            std::ofstream file(image_corrupted, std::ios::binary);
            file.write("\xFF\xD8\xFF", 3);  // Invalid JPEG data
            file.write("CORRUPTED_DATA", 14);
            file.close();
        }

        if (!fileExists(image_without_gps)) {
            std::ofstream file(image_without_gps);
            file.close();
        }
    }

    bool fileExists(const std::string& filename) {
        std::ifstream file(filename);
        return file.good();
    }

    std::string test_images_path;
    std::string image_with_gps_and_datetime;
    std::string image_without_gps;
    std::string image_without_datetime;
    std::string image_corrupted;
    std::string image_nonexistent;
};

TEST_F(EXIFDataTest, NonexistentImage_Constructor_ThrowsException) {
    EXPECT_THROW({ EXIFData data(image_nonexistent); }, std::invalid_argument);
}

TEST_F(EXIFDataTest, CorruptedImage_Constructor_ThrowsException) {
    EXPECT_THROW({ EXIFData data(image_corrupted); }, std::invalid_argument);
    std::remove(image_corrupted.c_str());
}

TEST_F(EXIFDataTest, ImageWithoutGPS_Constructor_ThrowsException) {
    EXPECT_THROW({ EXIFData data(image_without_gps); }, std::invalid_argument);
    std::remove(image_without_gps.c_str());
}

TEST_F(EXIFDataTest, ImageWithGPS_Constructor_NotThrowsException) {
    EXPECT_NO_THROW({ EXIFData data(image_with_gps_and_datetime); });
}

TEST_F(EXIFDataTest, ImageWithDatetime_Datetime_IsNotEmpty) {
    EXIFData data(image_with_gps_and_datetime);
    EXPECT_FALSE(data.get_datetime().empty());
}

TEST_F(EXIFDataTest, ImageWithDatetime_Datetime_IsNotDefault) {
    EXIFData data(image_with_gps_and_datetime);
    EXPECT_NE(data.get_datetime(), "No datetime");
}

TEST_F(EXIFDataTest, ImageWithDatetime_Datetime_HasCorrectLength) {
    EXIFData data(image_with_gps_and_datetime);
    EXPECT_EQ(data.get_datetime().length(), 19u);
}

TEST_F(EXIFDataTest, ImageWithDatetime_Datetime_HasCorrectContent) {
    EXIFData data(image_with_gps_and_datetime);
    std::regex pattern(R"(^\d{4}:\d{2}:\d{2} \d{2}:\d{2}:\d{2}$)");
    EXPECT_TRUE(std::regex_match(data.get_datetime(), pattern));
}

TEST_F(EXIFDataTest, ImageWithDatetime_DatetimeOriginal_IsNotEmpty) {
    EXIFData data(image_with_gps_and_datetime);
    EXPECT_FALSE(data.get_datetime_original().empty());
}

TEST_F(EXIFDataTest, ImageWithDatetime_DatetimeOriginal_IsNotDefault) {
    EXIFData data(image_with_gps_and_datetime);
    EXPECT_NE(data.get_datetime_original(), "No datetime");
}

TEST_F(EXIFDataTest, ImageWithDatetime_DatetimeOriginal_HasCorrectLength) {
    EXIFData data(image_with_gps_and_datetime);
    EXPECT_EQ(data.get_datetime_original().length(), 19u);
}

TEST_F(EXIFDataTest, ImageWithDatetime_DatetimeOriginal_HasCorrectContent) {
    EXIFData data(image_with_gps_and_datetime);
    std::regex pattern(R"(^\d{4}:\d{2}:\d{2} \d{2}:\d{2}:\d{2}$)");
    EXPECT_TRUE(std::regex_match(data.get_datetime_original(), pattern));
}

TEST_F(EXIFDataTest, ImageWithoutDatetime_Datetime_IsDefault) {
    EXIFData data(image_without_datetime);
    EXPECT_EQ(data.get_datetime(), "No datetime");
}

TEST_F(EXIFDataTest, ImageWithoutDatetime_DatetimeOriginal_IsDefault) {
    EXIFData data(image_without_datetime);
    EXPECT_EQ(data.get_datetime_original(), "No datetime");
}

TEST_F(EXIFDataTest, ImageWithGPS_GetLat_ReturnsExpectedValue) {
    EXIFData data(image_with_gps_and_datetime);
    double expected_lat = 56.2755737302778;
    double epsilon = 1e-8;
    EXPECT_NEAR(data.get_lat(), expected_lat, epsilon);
}

TEST_F(EXIFDataTest, ImageWithGPS_GetLon_ReturnsExpectedValue) {
    EXIFData data(image_with_gps_and_datetime);
    double expected_lon = 43.9779701230556;
    double epsilon = 1e-8;
    EXPECT_NEAR(data.get_lon(), expected_lon, epsilon);
}

TEST_F(EXIFDataTest, ImageWithGPS_Latitude_InValidRangeMin) {
    EXIFData data(image_with_gps_and_datetime);
    EXPECT_GE(data.get_lat(), -90.0);
}

TEST_F(EXIFDataTest, ImageWithGPS_Latitude_InValidRangeMax) {
    EXIFData data(image_with_gps_and_datetime);
    EXPECT_LE(data.get_lat(), 90.0);
}

TEST_F(EXIFDataTest, ImageWithGPS_Longitude_InValidRangeMin) {
    EXIFData data(image_with_gps_and_datetime);
    EXPECT_GE(data.get_lon(), -180.0);
}

TEST_F(EXIFDataTest, ImageWithGPS_Longitude_InValidRangeMax) {
    EXIFData data(image_with_gps_and_datetime);
    EXPECT_LE(data.get_lon(), 180.0);
}
