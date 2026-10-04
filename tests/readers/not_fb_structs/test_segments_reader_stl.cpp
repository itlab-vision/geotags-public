// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <variant>

#include "create_segments_reader.hpp"  // NOLINT

class SegmentsReaderSTLTest : public ::testing::Test {
 protected:
    void SetUp() override {
        valid_file = CreateTempFile(
            "valid",
            "1\n"
            "segment_id;lat_min;lat_max;lon_min;lon_max;neighbors;points\n"
            "10;1.0;2.0;3.0;4.0;11,12;100,101\n");

        empty_file = CreateTempFile("empty", "");

        only_headers_file = CreateTempFile(
            "only_headers",
            "0\nsegment_id;lat_min;lat_max;lon_min;lon_max;neighbors;points\n");

        carriage_returns_file = CreateTempFile(
            "carriage",
            "1\n"
            "segment_id;lat_min;lat_max;lon_min;lon_max;neighbors;points\n"
            "10;1.0;2.0;3.0;4.0;11,12;100,101\r\n");

        quotes_file = CreateTempFile(
            "quotes",
            "1\n"
            "segment_id;lat_min;lat_max;lon_min;lon_max;neighbors;points\n"
            "\"10\";\"1.0\";\"2.0\";\"3.0\";\"4.0\";\"11,12\";\"100,101\"\n");

        empty_lines_file = CreateTempFile(
            "empty_lines",
            "1\n"
            "segment_id;lat_min;lat_max;lon_min;lon_max;neighbors;points\n"
            "\n"
            "10;1.0;2.0;3.0;4.0;11,12;100,101\n"
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
        std::string filename = "tmp_seg_reader_stl_" + file_type + "_" +
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

TEST_F(SegmentsReaderSTLTest, Constructor_NonexistentFile_Throws) {
    EXPECT_THROW(create_segments_reader("csv", "nonexistent.csv"),
                 std::invalid_argument);
}

TEST_F(SegmentsReaderSTLTest, Read_EmptyFile_ReturnsEmpty) {
    auto reader_var = create_segments_reader("csv", empty_file);
    auto& reader = std::get<Ptr<SegmentsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.size(), 0u);
}

TEST_F(SegmentsReaderSTLTest, Read_OnlyHeadersFile_ReturnsEmpty) {
    auto reader_var = create_segments_reader("csv", only_headers_file);
    auto& reader = std::get<Ptr<SegmentsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.size(), 0u);
}

TEST_F(SegmentsReaderSTLTest, Read_ValidFile_ReturnsCorrectSize) {
    auto reader_var = create_segments_reader("csv", valid_file);
    auto& reader = std::get<Ptr<SegmentsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.size(), 1u);
}

TEST_F(SegmentsReaderSTLTest, Read_ValidFile_HasCorrectSegmentId) {
    auto reader_var = create_segments_reader("csv", valid_file);
    auto& reader = std::get<Ptr<SegmentsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.get_segment_id(0), 10u);
}

TEST_F(SegmentsReaderSTLTest, Read_ValidFile_HasCorrectLatMin) {
    auto reader_var = create_segments_reader("csv", valid_file);
    auto& reader = std::get<Ptr<SegmentsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_DOUBLE_EQ(wrapper.get_lat_min(0), 1.0);
}

TEST_F(SegmentsReaderSTLTest, Read_CarriageReturns_CorrectLatMin) {
    auto reader_var = create_segments_reader("csv", carriage_returns_file);
    auto& reader = std::get<Ptr<SegmentsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_DOUBLE_EQ(wrapper.get_lat_min(0), 1.0);
}

TEST_F(SegmentsReaderSTLTest, Read_Quotes_CorrectLatMin) {
    auto reader_var = create_segments_reader("csv", quotes_file);
    auto& reader = std::get<Ptr<SegmentsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_DOUBLE_EQ(wrapper.get_lat_min(0), 1.0);
}

TEST_F(SegmentsReaderSTLTest, Read_EmptyLines_ReturnsOnlyValid) {
    auto reader_var = create_segments_reader("csv", empty_lines_file);
    auto& reader = std::get<Ptr<SegmentsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.size(), 1u);
}
