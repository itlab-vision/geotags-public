// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <string>

#include "segments_parser.hpp"  // NOLINT

class SegmentsParserTest : public ::testing::Test {
 protected:
    SegmentsParser parser;

    const std::string valid_line = "10;1.0;2.0;3.0;4.0;11,12;100,101";
    const std::string quotes_line =
        "\"13\";\"5.0\";\"6.0\";\"7.0\";\"8.0\";\"14,15\";\"102\"";

    // Simulates Windows CRLF line ending after std::getline extracts '\n'
    const std::string carriage_line = "16;9.0;10.0;11.0;12.0;17;103\r";
};

TEST_F(SegmentsParserTest, ParseLine_ValidString_ReturnsTrue) {
    SegmentInfo item;
    EXPECT_TRUE(parser.parse_line(item, valid_line));
}

TEST_F(SegmentsParserTest, ParseLine_ValidString_CorrectId) {
    SegmentInfo item;
    parser.parse_line(item, valid_line);
    EXPECT_EQ(item.segment_id, 10u);
}

TEST_F(SegmentsParserTest, ParseLine_ValidString_CorrectLatMin) {
    SegmentInfo item;
    parser.parse_line(item, valid_line);
    EXPECT_DOUBLE_EQ(item.lat_range.first, 1.0);
}

TEST_F(SegmentsParserTest, ParseLine_ValidString_CorrectLatMax) {
    SegmentInfo item;
    parser.parse_line(item, valid_line);
    EXPECT_DOUBLE_EQ(item.lat_range.second, 2.0);
}

TEST_F(SegmentsParserTest, ParseLine_ValidString_CorrectNeighborsSize) {
    SegmentInfo item;
    parser.parse_line(item, valid_line);
    EXPECT_EQ(item.neighbors.size(), 2u);
}

TEST_F(SegmentsParserTest, ParseLine_ValidString_CorrectLocationsSize) {
    SegmentInfo item;
    parser.parse_line(item, valid_line);
    EXPECT_EQ(item.points.size(), 2u);
}

TEST_F(SegmentsParserTest, ParseLine_QuotesString_CorrectLatMin) {
    SegmentInfo item;
    parser.parse_line(item, quotes_line);
    EXPECT_DOUBLE_EQ(item.lat_range.first, 5.0);
}

TEST_F(SegmentsParserTest, ParseLine_CarriageString_CorrectLocationsSize) {
    SegmentInfo item;
    parser.parse_line(item, carriage_line);
    EXPECT_EQ(item.points.size(), 1u);
}

TEST_F(SegmentsParserTest, ParseLine_EmptyString_ReturnsFalse) {
    SegmentInfo item;
    EXPECT_FALSE(parser.parse_line(item, ""));
}
