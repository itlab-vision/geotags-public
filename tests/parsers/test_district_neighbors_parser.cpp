// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <string>

#include "district_neighbors_parser.hpp"  // NOLINT

class DistrictNeighborsParserTest : public ::testing::Test {
 protected:
    DistrictNeighborsParser parser;

    const std::string valid_line = "5;6,7,8";
    const std::string quotes_line = "\"9\";\"10,11\"";

    // Simulates Windows CRLF line ending after std::getline extracts '\n'
    const std::string carriage_line = "12;13\r";
};

TEST_F(DistrictNeighborsParserTest, ParseLine_ValidString_ReturnsTrue) {
    DistrictNeighborInfo item;
    EXPECT_TRUE(parser.parse_line(item, valid_line));
}

TEST_F(DistrictNeighborsParserTest, ParseLine_ValidString_CorrectId) {
    DistrictNeighborInfo item;
    parser.parse_line(item, valid_line);
    EXPECT_EQ(item.district_id, 5u);
}

TEST_F(DistrictNeighborsParserTest,
       ParseLine_ValidString_CorrectNeighborsSize) {
    DistrictNeighborInfo item;
    parser.parse_line(item, valid_line);
    EXPECT_EQ(item.neighbors.size(), 3u);
}

TEST_F(DistrictNeighborsParserTest,
       ParseLine_ValidString_CorrectFirstNeighbor) {
    DistrictNeighborInfo item;
    parser.parse_line(item, valid_line);
    EXPECT_EQ(item.neighbors.empty() ? 0 : item.neighbors[0], 6u);
}

TEST_F(DistrictNeighborsParserTest, ParseLine_QuotesString_CorrectId) {
    DistrictNeighborInfo item;
    parser.parse_line(item, quotes_line);
    EXPECT_EQ(item.district_id, 9u);
}

TEST_F(DistrictNeighborsParserTest,
       ParseLine_CarriageString_CorrectNeighborsSize) {
    DistrictNeighborInfo item;
    parser.parse_line(item, carriage_line);
    EXPECT_EQ(item.neighbors.size(), 1u);
}

TEST_F(DistrictNeighborsParserTest, ParseLine_EmptyString_ReturnsFalse) {
    DistrictNeighborInfo item;
    EXPECT_FALSE(parser.parse_line(item, ""));
}
