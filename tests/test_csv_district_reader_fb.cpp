// Copyright 2025 itlab-vision

#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "district_reader.hpp"  // NOLINT

class CSVDistrictsReaderFBTest : public ::testing::Test {
 protected:
    void SetUp() override {
        test_data_path = TEST_DATA_DIR;

        valid_test_file = CreateTempFile(
            "valid_districts",
            "id;name_en;name;wkt\n"
            "1;Moscow Oblast;Московская область;"
            "POLYGON((35.0 55.0, 35.0 56.0, 36.0 56.0, 36.0 55.0, 35.0 55.0))\n"
            "2;Tver Oblast;Тверская область;"
            "POLYGON((34.0 56.0, 34.0 57.0,35.0 57.0,"
            "35.0 56.0, 34.0 56.0))\n");

        quotes_in_data_file =
            CreateTempFile("quotes_in_data",
                           "id;name_en;name;wkt\n"
                           "\"1\";\"Moscow\" Oblast\";\"Московская\" область\";"
                           "\"POLYGON((35.0 55.0, 35.0 56.0, 36.0 56.0,"
                           "36.0 55.0, 35.0 55.0))\"\n");

        with_carriage_returns_file =
            CreateTempFile("with_carriage_returns",
                           "id;name_en;name;wkt\r\n"
                           "1;Moscow Oblast;Московская область;"
                           "POLYGON((35.0 55.0, 35.0 56.0, 36.0 56.0,"
                           "36.0 55.0, 35.0 55.0))\r\n");

        different_non_polygon_geom_types_file = CreateTempFile(
            "different_non_polygon_geom_types",
            "id;name_en;name;wkt\n"
            "1;Polygon Region;Полигон;"
            "POLYGON((35.0 55.0, 35.0 56.0, 36.0 56.0, 36.0 55.0,"
            "35.0 55.0))\n"
            "2;Point Region;Точка;POINT(35.0 55.0)\n"
            "3;Line Region;Линия;LINESTRING(35.0 55.0, 36.0 56.0)\n"
            "4;MultiPolygon Region;Мультиполигон;"
            "MULTIPOLYGON((35.0 55.0, 35.0 56.0, 36.0 56.0,"
            "36.0 55.0, 35.0 55.0))\n");

        different_polygon_geom_types_file = CreateTempFile(
            "different_polygon_geom_types",
            "id;name_en;name;wkt\n"
            "1;Polygon Region;Полигон;"
            "POLYGON((35.0 55.0, 35.0 56.0, 36.0 56.0, 36.0 55.0,"
            "35.0 55.0))\n"
            "2;MultiPolygon Region;Мультиполигон;"
            "MULTIPOLYGON (((30 20, 45 40, 10 40, 30 20)),"
            "((15 5, 40 10, 10 20, 5 10, 15 5),"
            "(20 15, 35 15, 25 25, 20 15)))\n");
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

    void CleanupDistricts(
        std::vector<std::pair<DistrictInfo, tg_geom*>>& districts) {
        for (auto& district : districts) {
            if (district.second) {
                tg_geom_free(district.second);
            }
        }
        districts.clear();
    }

    bool fileExists(const std::string& filename) {
        std::ifstream file(filename);
        return file.good();
    }

    std::vector<std::string> test_files;
    std::string test_data_path;

    std::string valid_test_file;
    std::string quotes_in_data_file;
    std::string with_carriage_returns_file;
    std::string different_non_polygon_geom_types_file;
    std::string different_polygon_geom_types_file;
};

TEST_F(CSVDistrictsReaderFBTest, ValidFile_Constructor_DoesNotThrowException) {
    EXPECT_NO_THROW(CSVDistrictsReaderFB reader(valid_test_file));
}

TEST_F(CSVDistrictsReaderFBTest, NonexistentFile_Constructor_ThrowsException) {
    EXPECT_THROW(
        { CSVDistrictsReaderFB reader("nonexistent_dist_file.csv"); },
        std::invalid_argument);
}

TEST_F(CSVDistrictsReaderFBTest, EmptyFile_Read_ReturnsEmptyDistricts) {
    std::string empty_file = CreateTempFile("empty", "id;name_en;name;wkt\n");
    CSVDistrictsReaderFB reader(empty_file);
    auto districts = reader.read();
    EXPECT_TRUE(districts.empty());
    CleanupDistricts(districts);
}

TEST_F(CSVDistrictsReaderFBTest, OnlyHeadersFile_Read_ReturnsEmptyDistricts) {
    std::string only_headers_file =
        CreateTempFile("only_headers", "id;name_en;name;wkt\n");
    CSVDistrictsReaderFB reader(only_headers_file);
    auto districts = reader.read();
    EXPECT_TRUE(districts.empty());
    CleanupDistricts(districts);
}

TEST_F(CSVDistrictsReaderFBTest, EmptyLinesFile_Read_ThrowException) {
    std::string empty_lines_file = CreateTempFile(
        "empty_lines",
        "id;name_en;name;wkt\n"
        "\n"
        "1;Moscow Oblast;Московская область;"
        "POLYGON((35.0 55.0, 35.0 56.0, 36.0 56.0, 36.0 55.0, 35.0 55.0))\n"
        ";;;\n");
    CSVDistrictsReaderFB reader(empty_lines_file);
    EXPECT_THROW(reader.read(), std::invalid_argument);
}

TEST_F(CSVDistrictsReaderFBTest,
       ValidFile_Read_ReturnsCorrectNumberOfDistricts) {
    CSVDistrictsReaderFB reader(valid_test_file);
    auto districts = reader.read();
    EXPECT_EQ(districts.size(), 2);
    CleanupDistricts(districts);
}

TEST_F(CSVDistrictsReaderFBTest,
       ValidFile_FirstDistrict_HasCorrectEnglishName) {
    CSVDistrictsReaderFB reader(valid_test_file);
    auto districts = reader.read();
    EXPECT_EQ(districts[0].first.name_en, "Moscow Oblast");
    CleanupDistricts(districts);
}

TEST_F(CSVDistrictsReaderFBTest, ValidFile_FirstDistrict_HasValidGeometry) {
    CSVDistrictsReaderFB reader(valid_test_file);
    auto districts = reader.read();
    EXPECT_FALSE(tg_geom_error(districts[0].second));
    CleanupDistricts(districts);
}

TEST_F(CSVDistrictsReaderFBTest, ValidFile_FirstDistrict_HasCorrectLocalName) {
    CSVDistrictsReaderFB reader(valid_test_file);
    auto districts = reader.read();
    EXPECT_EQ(districts[0].first.name, "Московская область");
    CleanupDistricts(districts);
}

TEST_F(CSVDistrictsReaderFBTest, ValidFile_FirstDistrict_HasCorrectID) {
    CSVDistrictsReaderFB reader(valid_test_file);
    auto districts = reader.read();
    EXPECT_EQ(districts[0].first.id, 1);
    CleanupDistricts(districts);
}

TEST_F(CSVDistrictsReaderFBTest, ValidFile_SecondDistrict_HasCorrectID) {
    CSVDistrictsReaderFB reader(valid_test_file);
    auto districts = reader.read();
    EXPECT_EQ(districts[1].first.id, 2);
    CleanupDistricts(districts);
}

TEST_F(CSVDistrictsReaderFBTest,
       ValidFile_SecondDistrict_HasCorrectEnglishName) {
    CSVDistrictsReaderFB reader(valid_test_file);
    auto districts = reader.read();
    EXPECT_EQ(districts[1].first.name_en, "Tver Oblast");
    CleanupDistricts(districts);
}

TEST_F(CSVDistrictsReaderFBTest, ValidFile_SecondDistrict_HasCorrectLocalName) {
    CSVDistrictsReaderFB reader(valid_test_file);
    auto districts = reader.read();
    EXPECT_EQ(districts[1].first.name, "Тверская область");
    CleanupDistricts(districts);
}

TEST_F(CSVDistrictsReaderFBTest, ValidFile_SecondDistrict_HasValidGeometry) {
    CSVDistrictsReaderFB reader(valid_test_file);
    auto districts = reader.read();
    EXPECT_FALSE(tg_geom_error(districts[1].second));
    CleanupDistricts(districts);
}

TEST_F(CSVDistrictsReaderFBTest,
       FileCarriageReturns_EnglishName_HasNoCarriageReturns) {
    CSVDistrictsReaderFB reader(with_carriage_returns_file);
    auto districts = reader.read();
    EXPECT_EQ(districts[0].first.name_en.find('\r'), std::string::npos);
    CleanupDistricts(districts);
}

TEST_F(CSVDistrictsReaderFBTest,
       FileCarriageReturns_LocalName_HasNoCarriageReturns) {
    CSVDistrictsReaderFB reader(with_carriage_returns_file);
    auto districts = reader.read();
    EXPECT_EQ(districts[0].first.name.find('\r'), std::string::npos);
    CleanupDistricts(districts);
}

TEST_F(CSVDistrictsReaderFBTest, FileWithQuotes_EnglishName_HaveNoQuotes) {
    CSVDistrictsReaderFB reader(quotes_in_data_file);
    auto districts = reader.read();
    EXPECT_EQ(districts[0].first.name_en.find('\"'), std::string::npos);
    CleanupDistricts(districts);
}

TEST_F(CSVDistrictsReaderFBTest, FileWithQuotes_LocalName_HaveNoQuotes) {
    CSVDistrictsReaderFB reader(quotes_in_data_file);
    auto districts = reader.read();
    EXPECT_EQ(districts[0].first.name.find('\"'), std::string::npos);
    CleanupDistricts(districts);
}

TEST_F(CSVDistrictsReaderFBTest, InvalidWKTFile_Read_ThrowException) {
    std::string invalid_wkt_file =
        CreateTempFile("invalid_wkt",
                       "id;name_en;name;wkt\n"
                       "1;Test Region;Тестовый регион;INVALID WKT\n");
    CSVDistrictsReaderFB reader(invalid_wkt_file);
    EXPECT_THROW(reader.read(), std::invalid_argument);
}

TEST_F(CSVDistrictsReaderFBTest, MalformedDataFile_Read_ThrowException) {
    std::string malformed_data_file =
        CreateTempFile("malformed_data",
                       "id;name_en;name;wkt\n"
                       "1;Moscow Oblast;Московская область\n"
                       "2;Tver Oblast;Тверская область;POLYGON(("
                       "34.0 56.0, 34.0 57.0, 35.0 57.0))\n");
    CSVDistrictsReaderFB reader(malformed_data_file);
    EXPECT_THROW(reader.read(), std::invalid_argument);
}

TEST_F(CSVDistrictsReaderFBTest,
       DifferentNonPolygonGeometryTypes_Read_ThrowException) {
    CSVDistrictsReaderFB reader(different_non_polygon_geom_types_file);
    EXPECT_THROW(reader.read(), std::invalid_argument);
}

TEST_F(CSVDistrictsReaderFBTest,
       DifferentPolygonGeometryTypes_Read_NoThrowException) {
    CSVDistrictsReaderFB reader(different_polygon_geom_types_file);
    EXPECT_NO_THROW({
        auto districts = reader.read();
        CleanupDistricts(districts);
    });
}

TEST_F(CSVDistrictsReaderFBTest,
       DifferentPolygonGeometryTypes_Polygon_IsValid) {
    CSVDistrictsReaderFB reader(different_polygon_geom_types_file);
    auto districts = reader.read();
    EXPECT_FALSE(tg_geom_error(districts[0].second));
    CleanupDistricts(districts);
}

TEST_F(CSVDistrictsReaderFBTest,
       DifferentPolygonGeometryTypes_MultiPolygon_IsValid) {
    CSVDistrictsReaderFB reader(different_polygon_geom_types_file);
    auto districts = reader.read();
    EXPECT_FALSE(tg_geom_error(districts[1].second));
    CleanupDistricts(districts);
}

TEST_F(CSVDistrictsReaderFBTest, Read_CreatesFBFile) {
    CSVDistrictsReaderFB reader(valid_test_file);
    std::string fb_file = valid_test_file + ".fb";
    if (fileExists(fb_file)) std::remove(fb_file.c_str());
    (void)reader.read();
    EXPECT_TRUE(fileExists(fb_file));
}
