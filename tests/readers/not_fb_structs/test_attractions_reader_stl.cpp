// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <variant>

#include "create_attractions_reader.hpp"  // NOLINT

class AttractionsReaderSTLTest : public ::testing::Test {
 protected:
    void SetUp() override {
        valid_file = CreateTempFile("valid",
                                    "1\n"
                                    "lon;lat;type;name;visited;entry\n"
                                    "37.61;55.75;museum;Kremlin;yes;free\n");

        empty_file = CreateTempFile("empty", "");

        only_headers_file = CreateTempFile(
            "only_headers", "0\nlon;lat;type;name;visited;entry\n");

        carriage_returns_file =
            CreateTempFile("carriage",
                           "1\n"
                           "lon;lat;type;name;visited;entry\n"
                           "37.61;55.75;museum;Kremlin;yes;free\r\n");

        quotes_file = CreateTempFile(
            "quotes",
            "1\n"
            "lon;lat;type;name;visited;entry\n"
            "\"37.61\";\"55.75\";\"museum\";\"Kremlin\";\"yes\";\"free\"\n");

        empty_lines_file =
            CreateTempFile("empty_lines",
                           "1\n"
                           "lon;lat;type;name;visited;entry\n"
                           "\n"
                           "37.61;55.75;museum;Kremlin;yes;free\n"
                           "\n");
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
        std::string filename = "tmp_attr_reader_stl_" + file_type + "_" +
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
};

TEST_F(AttractionsReaderSTLTest, Constructor_NonexistentFile_Throws) {
    EXPECT_THROW(create_attractions_reader("csv", "nonexistent.csv"),
                 std::invalid_argument);
}

TEST_F(AttractionsReaderSTLTest, Read_EmptyFile_ReturnsEmpty) {
    auto reader_var = create_attractions_reader("csv", empty_file);
    auto& reader = std::get<Ptr<AttractionsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.size(), 0u);
}

TEST_F(AttractionsReaderSTLTest, Read_OnlyHeadersFile_ReturnsEmpty) {
    auto reader_var = create_attractions_reader("csv", only_headers_file);
    auto& reader = std::get<Ptr<AttractionsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.size(), 0u);
}

TEST_F(AttractionsReaderSTLTest, Read_ValidFile_ReturnsCorrectSize) {
    auto reader_var = create_attractions_reader("csv", valid_file);
    auto& reader = std::get<Ptr<AttractionsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.size(), 1u);
}

TEST_F(AttractionsReaderSTLTest, Read_ValidFile_HasCorrectLat) {
    auto reader_var = create_attractions_reader("csv", valid_file);
    auto& reader = std::get<Ptr<AttractionsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_DOUBLE_EQ(wrapper.get_lat(0), 55.75);
}

TEST_F(AttractionsReaderSTLTest, Read_ValidFile_HasCorrectName) {
    auto reader_var = create_attractions_reader("csv", valid_file);
    auto& reader = std::get<Ptr<AttractionsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.get_info(0).name, "Kremlin");
}

TEST_F(AttractionsReaderSTLTest, Read_CarriageReturns_CorrectName) {
    auto reader_var = create_attractions_reader("csv", carriage_returns_file);
    auto& reader = std::get<Ptr<AttractionsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.get_info(0).name, "Kremlin");
}

TEST_F(AttractionsReaderSTLTest, Read_Quotes_CorrectName) {
    auto reader_var = create_attractions_reader("csv", quotes_file);
    auto& reader = std::get<Ptr<AttractionsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.get_info(0).name, "Kremlin");
}

TEST_F(AttractionsReaderSTLTest, Read_EmptyLines_ReturnsOnlyValid) {
    auto reader_var = create_attractions_reader("csv", empty_lines_file);
    auto& reader = std::get<Ptr<AttractionsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.size(), 1u);
}
