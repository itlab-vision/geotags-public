// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <variant>

#include "create_district_neighbors_reader.hpp"  // NOLINT

class DistrictNeighborsReaderSTLTest : public ::testing::Test {
 protected:
    void SetUp() override {
        valid_file = CreateTempFile("valid",
                                    "1\n"
                                    "district_id;neighbors\n"
                                    "5;6,7,8\n");

        empty_file = CreateTempFile("empty", "");

        only_headers_file =
            CreateTempFile("only_headers", "0\ndistrict_id;neighbors\n");

        carriage_returns_file = CreateTempFile("carriage",
                                               "1\n"
                                               "district_id;neighbors\n"
                                               "5;6,7,8\r\n");

        quotes_file = CreateTempFile("quotes",
                                     "1\n"
                                     "district_id;neighbors\n"
                                     "\"5\";\"6,7,8\"\n");

        empty_lines_file = CreateTempFile("empty_lines",
                                          "1\n"
                                          "district_id;neighbors\n"
                                          "\n"
                                          "5;6,7,8\n"
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
        std::string filename = "tmp_nbr_reader_stl_" + file_type + "_" +
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

TEST_F(DistrictNeighborsReaderSTLTest, Constructor_NonexistentFile_Throws) {
    EXPECT_THROW(create_district_neighbors_reader("csv", "nonexistent.csv"),
                 std::invalid_argument);
}

TEST_F(DistrictNeighborsReaderSTLTest, Read_EmptyFile_ReturnsEmpty) {
    auto reader_var = create_district_neighbors_reader("csv", empty_file);
    auto& reader = std::get<Ptr<DistrictNeighborsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.size(), 0u);
}

TEST_F(DistrictNeighborsReaderSTLTest, Read_OnlyHeadersFile_ReturnsEmpty) {
    auto reader_var =
        create_district_neighbors_reader("csv", only_headers_file);
    auto& reader = std::get<Ptr<DistrictNeighborsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.size(), 0u);
}

TEST_F(DistrictNeighborsReaderSTLTest, Read_ValidFile_ReturnsCorrectSize) {
    auto reader_var = create_district_neighbors_reader("csv", valid_file);
    auto& reader = std::get<Ptr<DistrictNeighborsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.size(), 1u);
}

TEST_F(DistrictNeighborsReaderSTLTest, Read_ValidFile_CorrectNeighborsSize) {
    auto reader_var = create_district_neighbors_reader("csv", valid_file);
    auto& reader = std::get<Ptr<DistrictNeighborsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    std::vector<unsigned int> nbrs;
    wrapper.get_nbrs(0, nbrs);
    EXPECT_EQ(nbrs.size(), 3u);
}

TEST_F(DistrictNeighborsReaderSTLTest, Read_ValidFile_CorrectFirstNeighbor) {
    auto reader_var = create_district_neighbors_reader("csv", valid_file);
    auto& reader = std::get<Ptr<DistrictNeighborsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    std::vector<unsigned int> nbrs;
    wrapper.get_nbrs(0, nbrs);
    EXPECT_EQ(nbrs[0], 6u);
}

TEST_F(DistrictNeighborsReaderSTLTest,
       Read_CarriageReturns_CorrectNeighborsSize) {
    auto reader_var =
        create_district_neighbors_reader("csv", carriage_returns_file);
    auto& reader = std::get<Ptr<DistrictNeighborsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    std::vector<unsigned int> nbrs;
    wrapper.get_nbrs(0, nbrs);
    EXPECT_EQ(nbrs.size(), 3u);
}

TEST_F(DistrictNeighborsReaderSTLTest, Read_Quotes_CorrectNeighborsSize) {
    auto reader_var = create_district_neighbors_reader("csv", quotes_file);
    auto& reader = std::get<Ptr<DistrictNeighborsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    std::vector<unsigned int> nbrs;
    wrapper.get_nbrs(0, nbrs);
    EXPECT_EQ(nbrs.size(), 3u);
}

TEST_F(DistrictNeighborsReaderSTLTest, Read_EmptyLines_ReturnsOnlyValid) {
    auto reader_var = create_district_neighbors_reader("csv", empty_lines_file);
    auto& reader = std::get<Ptr<DistrictNeighborsReaderSTL>>(reader_var);
    auto wrapper = reader->read();
    EXPECT_EQ(wrapper.size(), 1u);
}
