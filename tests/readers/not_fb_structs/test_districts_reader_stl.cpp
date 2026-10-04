// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <variant>

#include "create_districts_reader.hpp"  // NOLINT

class DistrictsReaderSTLTest : public ::testing::Test {
 protected:
    void SetUp() override {
        valid_file =
            CreateTempFile("valid",
                           "1\n"
                           "id;name_en;name;wkt\n"
                           "1;Moscow_District;Московский район;POLYGON((0 0, 1 "
                           "0, 1 1, 0 1, 0 0))\n");

        empty_file = CreateTempFile("empty", "");

        only_headers_file =
            CreateTempFile("only_headers", "0\nid;name_en;name;wkt\n");

        carriage_returns_file =
            CreateTempFile("carriage",
                           "1\n"
                           "id;name_en;name;wkt\n"
                           "2;Tver_District;Тверской район;POLYGON((0 0, 1 0, "
                           "1 1, 0 1, 0 0))\r\n");

        quotes_file =
            CreateTempFile("quotes",
                           "1\n"
                           "id;name_en;name;wkt\n"
                           "\"3\";\"Kazan_District\";\"Казанский "
                           "район\";\"POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))\"\n");

        empty_lines_file =
            CreateTempFile("empty_lines",
                           "1\n"
                           "id;name_en;name;wkt\n"
                           "\n"
                           "1;Moscow_District;Московский район;POLYGON((0 0, 1 "
                           "0, 1 1, 0 1, 0 0))\n"
                           "\n");

        invalid_wkt_file = CreateTempFile("invalid_wkt",
                                          "1\n"
                                          "id;name_en;name;wkt\n"
                                          "4;Test;Тест;INVALID_WKT\n");
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
        std::string filename = "tmp_dist_reader_stl_" + file_type + "_" +
                               std::to_string(counters[file_type]++) + ".csv";
        std::ofstream file(filename);
        if (!file) throw std::runtime_error("Cannot create temp file");
        file << content;
        file.close();
        test_files.push_back(filename);
        return filename;
    }

    std::vector<std::string> test_files;
    std::string valid_file;
    std::string empty_file;
    std::string only_headers_file;
    std::string carriage_returns_file;
    std::string quotes_file;
    std::string empty_lines_file;
    std::string invalid_wkt_file;
};

TEST_F(DistrictsReaderSTLTest, Constructor_NonexistentFile_Throws) {
    EXPECT_THROW(create_districts_reader("csv", "nonexistent.csv"),
                 std::invalid_argument);
}

TEST_F(DistrictsReaderSTLTest, Read_EmptyFile_ReturnsEmpty) {
    auto reader_var = create_districts_reader("csv", empty_file);
    auto& reader = std::get<Ptr<DistrictsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.size(), 0u);
}

TEST_F(DistrictsReaderSTLTest, Read_OnlyHeadersFile_ReturnsEmpty) {
    auto reader_var = create_districts_reader("csv", only_headers_file);
    auto& reader = std::get<Ptr<DistrictsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.size(), 0u);
}

TEST_F(DistrictsReaderSTLTest, Read_ValidFile_ReturnsCorrectSize) {
    auto reader_var = create_districts_reader("csv", valid_file);
    auto& reader = std::get<Ptr<DistrictsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.size(), 1u);
}

TEST_F(DistrictsReaderSTLTest, Read_ValidFile_HasCorrectId) {
    auto reader_var = create_districts_reader("csv", valid_file);
    auto& reader = std::get<Ptr<DistrictsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.get_info(0).id, 1u);
}

TEST_F(DistrictsReaderSTLTest, Read_ValidFile_HasCorrectEnName) {
    auto reader_var = create_districts_reader("csv", valid_file);
    auto& reader = std::get<Ptr<DistrictsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.get_info(0).name_en, "Moscow_District");
}

TEST_F(DistrictsReaderSTLTest, Read_ValidFile_GeomNotNull) {
    auto reader_var = create_districts_reader("csv", valid_file);
    auto& reader = std::get<Ptr<DistrictsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_NE(wrapper.get_geom(0), nullptr);
}

TEST_F(DistrictsReaderSTLTest, Read_CarriageReturns_CorrectEnName) {
    auto reader_var = create_districts_reader("csv", carriage_returns_file);
    auto& reader = std::get<Ptr<DistrictsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.get_info(0).name_en, "Tver_District");
}

TEST_F(DistrictsReaderSTLTest, Read_Quotes_CorrectEnName) {
    auto reader_var = create_districts_reader("csv", quotes_file);
    auto& reader = std::get<Ptr<DistrictsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.get_info(0).name_en, "Kazan_District");
}

TEST_F(DistrictsReaderSTLTest, Read_EmptyLines_ReturnsOnlyValid) {
    auto reader_var = create_districts_reader("csv", empty_lines_file);
    auto& reader = std::get<Ptr<DistrictsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.size(), 1u);
}

TEST_F(DistrictsReaderSTLTest, Read_InvalidWKT_ThrowsException) {
    auto reader_var = create_districts_reader("csv", invalid_wkt_file);
    auto& reader = std::get<Ptr<DistrictsReaderSTL>>(reader_var);
    EXPECT_THROW(reader->read(), std::invalid_argument);
}
