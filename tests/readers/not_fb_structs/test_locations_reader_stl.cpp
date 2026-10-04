// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <variant>

#include "create_locations_reader.hpp"  // NOLINT

class LocationsReaderSTLTest : public ::testing::Test {
 protected:
    void SetUp() override {
        city_test_file =
            CreateTempFile("city_data",
                           "2\n"
                           "id;country;city;lat;lon\n"
                           "0;\"Russia\";\"Moscow\";55.7558;37.6173\n"
                           "1;\"Germany\";\"Berlin\";52.5200;13.4050\n");

        empty_file = CreateTempFile("empty", "");

        only_headers_file =
            CreateTempFile("only_headers", "0\nid;country;city;lat;lon\n");

        with_carriage_returns_file =
            CreateTempFile("with_carriage",
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
                           "2\n"
                           "id;country;city;lat;lon\n"
                           "0;\"MinCoord\";\"Test1\";-90.0;-180.0\n"
                           "1;\"MaxCoord\";\"Test2\";90.0;180.0\n");

        different_number_formats_file =
            CreateTempFile("different_number_formats",
                           "3\n"
                           "id;country;city;lat;lon\n"
                           "0;\"Test1\";\"City1\";55;37\n"
                           "1;\"Test2\";\"City2\";55.7558;37.6173\n"
                           "2;\"Test3\";\"City3\";-45.1234;123.4567\n");
    }

    void TearDown() override {
        for (const auto& filename : test_files) {
            std::remove(filename.c_str());
        }
        test_files.clear();
    }

    std::string CreateTempFile(const std::string& file_type,
                               const std::string& content) {
        static std::unordered_map<std::string, int> counters;
        std::string filename = "tmp_loc_reader_stl_" + file_type + "_" +
                               std::to_string(counters[file_type]++) + ".csv";
        std::ofstream file(filename);
        if (!file) throw std::runtime_error("Cannot create temp file");
        file << content;
        file.close();
        test_files.push_back(filename);
        return filename;
    }

    std::vector<std::string> test_files;
    std::string city_test_file;
    std::string empty_file;
    std::string only_headers_file;
    std::string with_carriage_returns_file;
    std::string quotes_in_data_file;
    std::string extra_commas_file;
    std::string empty_lines_file;
    std::string boundary_coordinates_file;
    std::string different_number_formats_file;
};

TEST_F(LocationsReaderSTLTest, Constructor_NonexistentFile_Throws) {
    EXPECT_THROW(create_locations_reader("csv", "nonexistent.csv"),
                 std::invalid_argument);
}

TEST_F(LocationsReaderSTLTest, Read_EmptyFile_ReturnsEmpty) {
    auto reader_var = create_locations_reader("csv", empty_file);
    auto& reader = std::get<Ptr<LocationsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.size(), 0u);
}

TEST_F(LocationsReaderSTLTest, Read_OnlyHeadersFile_ReturnsEmpty) {
    auto reader_var = create_locations_reader("csv", only_headers_file);
    auto& reader = std::get<Ptr<LocationsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.size(), 0u);
}

TEST_F(LocationsReaderSTLTest, Read_CityFile_ReturnsCorrectSize) {
    auto reader_var = create_locations_reader("csv", city_test_file);
    auto& reader = std::get<Ptr<LocationsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.size(), 2u);
}

TEST_F(LocationsReaderSTLTest, Read_CityFile_HasCorrectCountry) {
    auto reader_var = create_locations_reader("csv", city_test_file);
    auto& reader = std::get<Ptr<LocationsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.get_info(0).country, "Russia");
}

TEST_F(LocationsReaderSTLTest, Read_CityFile_HasCorrectCity) {
    auto reader_var = create_locations_reader("csv", city_test_file);
    auto& reader = std::get<Ptr<LocationsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.get_info(0).city, "Moscow");
}

TEST_F(LocationsReaderSTLTest, Read_CityFile_HasCorrectLatitude) {
    auto reader_var = create_locations_reader("csv", city_test_file);
    auto& reader = std::get<Ptr<LocationsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_DOUBLE_EQ(wrapper.get_lat(0), 55.7558);
}

TEST_F(LocationsReaderSTLTest, Read_CityFile_HasCorrectLongitude) {
    auto reader_var = create_locations_reader("csv", city_test_file);
    auto& reader = std::get<Ptr<LocationsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_DOUBLE_EQ(wrapper.get_lon(0), 37.6173);
}

TEST_F(LocationsReaderSTLTest, Read_CarriageReturns_CorrectCity) {
    auto reader_var =
        create_locations_reader("csv", with_carriage_returns_file);
    auto& reader = std::get<Ptr<LocationsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.get_info(0).city, "Moscow");
}

TEST_F(LocationsReaderSTLTest, Read_CarriageReturns_CorrectCountry) {
    auto reader_var =
        create_locations_reader("csv", with_carriage_returns_file);
    auto& reader = std::get<Ptr<LocationsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.get_info(0).country, "Russia");
}

TEST_F(LocationsReaderSTLTest, Read_Quotes_CorrectCity) {
    auto reader_var = create_locations_reader("csv", quotes_in_data_file);
    auto& reader = std::get<Ptr<LocationsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.get_info(0).city, "Moscow");
}

TEST_F(LocationsReaderSTLTest, Read_Quotes_CorrectCountry) {
    auto reader_var = create_locations_reader("csv", quotes_in_data_file);
    auto& reader = std::get<Ptr<LocationsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.get_info(0).country, "Russia");
}

TEST_F(LocationsReaderSTLTest, Read_ExtraColumns_CorrectCity) {
    auto reader_var = create_locations_reader("csv", extra_commas_file);
    auto& reader = std::get<Ptr<LocationsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.get_info(0).city, "Moscow");
}

TEST_F(LocationsReaderSTLTest, Read_EmptyLines_ReturnsOnlyValid) {
    auto reader_var = create_locations_reader("csv", empty_lines_file);
    auto& reader = std::get<Ptr<LocationsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.size(), 1u);
}

TEST_F(LocationsReaderSTLTest, Read_BoundaryCoordinates_MinLatitude) {
    auto reader_var = create_locations_reader("csv", boundary_coordinates_file);
    auto& reader = std::get<Ptr<LocationsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_DOUBLE_EQ(wrapper.get_lat(0), -90.0);
}

TEST_F(LocationsReaderSTLTest, Read_BoundaryCoordinates_MinLongitude) {
    auto reader_var = create_locations_reader("csv", boundary_coordinates_file);
    auto& reader = std::get<Ptr<LocationsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_DOUBLE_EQ(wrapper.get_lon(0), -180.0);
}

TEST_F(LocationsReaderSTLTest, Read_BoundaryCoordinates_MaxLatitude) {
    auto reader_var = create_locations_reader("csv", boundary_coordinates_file);
    auto& reader = std::get<Ptr<LocationsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_DOUBLE_EQ(wrapper.get_lat(1), 90.0);
}

TEST_F(LocationsReaderSTLTest, Read_BoundaryCoordinates_MaxLongitude) {
    auto reader_var = create_locations_reader("csv", boundary_coordinates_file);
    auto& reader = std::get<Ptr<LocationsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_DOUBLE_EQ(wrapper.get_lon(1), 180.0);
}

TEST_F(LocationsReaderSTLTest, Read_DifferentNumberFormats_Size) {
    auto reader_var =
        create_locations_reader("csv", different_number_formats_file);
    auto& reader = std::get<Ptr<LocationsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.size(), 3u);
}
