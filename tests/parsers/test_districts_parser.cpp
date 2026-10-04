// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <string>

#include "districts_parser.hpp"  // NOLINT

class DistrictsParserTest : public ::testing::Test {
 protected:
    DistrictsParser parser;

    const std::string valid_line =
        "1;Moscow_District;Московский район;POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))";
    const std::string quotes_line =
        "\"3\";\"Kazan_District\";\"Казанский район\";\"POLYGON((0 0, 1 0, 1 "
        "1, 0 1, 0 0))\"";
    const std::string invalid_wkt_line = "4;Test;Тест;INVALID_WKT";

    // Simulates Windows CRLF line ending after std::getline extracts '\n'
    const std::string carriage_line =
        "2;Tver_District;Тверской район;POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))\r";
};

TEST_F(DistrictsParserTest, ParseLine_ValidString_ReturnsTrue) {
    DistrictItem item;
    EXPECT_TRUE(parser.parse_line(item, valid_line));
}

TEST_F(DistrictsParserTest, ParseLine_ValidString_CorrectId) {
    DistrictItem item;
    parser.parse_line(item, valid_line);
    EXPECT_EQ(item.first.id, 1u);
}

TEST_F(DistrictsParserTest, ParseLine_ValidString_CorrectEnName) {
    DistrictItem item;
    parser.parse_line(item, valid_line);
    EXPECT_EQ(item.first.name_en, "Moscow_District");
}

TEST_F(DistrictsParserTest, ParseLine_ValidString_CorrectRuName) {
    DistrictItem item;
    parser.parse_line(item, valid_line);
    EXPECT_EQ(item.first.name, "Московский район");
}

TEST_F(DistrictsParserTest, ParseLine_ValidString_GeometryNotNull) {
    DistrictItem item;
    parser.parse_line(item, valid_line);
    EXPECT_NE(item.second, nullptr);
}

TEST_F(DistrictsParserTest, ParseLine_CarriageString_CorrectEnName) {
    DistrictItem item;
    parser.parse_line(item, carriage_line);
    EXPECT_EQ(item.first.name_en, "Tver_District");
}

TEST_F(DistrictsParserTest, ParseLine_CarriageString_GeometryNotNull) {
    DistrictItem item;
    parser.parse_line(item, carriage_line);
    EXPECT_NE(item.second, nullptr);
}

TEST_F(DistrictsParserTest, ParseLine_QuotesString_CorrectEnName) {
    DistrictItem item;
    parser.parse_line(item, quotes_line);
    EXPECT_EQ(item.first.name_en, "Kazan_District");
}

TEST_F(DistrictsParserTest, ParseLine_QuotesString_GeometryNotNull) {
    DistrictItem item;
    parser.parse_line(item, quotes_line);
    EXPECT_NE(item.second, nullptr);
}

TEST_F(DistrictsParserTest, ParseLine_InvalidWKTString_ThrowsException) {
    DistrictItem item;
    EXPECT_THROW(parser.parse_line(item, invalid_wkt_line),
                 std::invalid_argument);
}

TEST_F(DistrictsParserTest, ParseLine_EmptyString_ReturnsFalse) {
    DistrictItem item;
    EXPECT_FALSE(parser.parse_line(item, ""));
}
