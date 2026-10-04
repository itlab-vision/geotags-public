// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <string>
#include <unordered_map>

#include "attractions_parser.hpp"  // NOLINT

class AttractionsParserTest : public ::testing::Test {
 protected:
    AttractionsParser parser;

    const std::string valid_header = "lon;lat;type;name;visited;entry";
    const std::string valid_line = "37.61;55.75;museum;Kremlin;yes;free";

    const std::string quotes_header = "lon;lat;type;name;visited";
    const std::string quotes_line =
        "\"30.33\";\"59.93\";\"park\";\"Peterhof\";\"no\"";

    // Simulates Windows CRLF line ending after std::getline extracts '\n'
    const std::string carriage_line = "49.10;55.79;monument;Kazan Cat;yes\r";
};

TEST_F(AttractionsParserTest, ParseLine_ValidString_ReturnsTrue) {
    parser.init_parser(valid_header);
    AttractionItem item;

    EXPECT_TRUE(parser.parse_line(item, valid_line));
}

TEST_F(AttractionsParserTest, ParseLine_ValidString_CorrectLon) {
    parser.init_parser(valid_header);
    AttractionItem item;
    parser.parse_line(item, valid_line);

    EXPECT_DOUBLE_EQ(item.second.longitude, 37.61);
}

TEST_F(AttractionsParserTest, ParseLine_ValidString_CorrectLat) {
    parser.init_parser(valid_header);
    AttractionItem item;
    parser.parse_line(item, valid_line);

    EXPECT_DOUBLE_EQ(item.second.latitude, 55.75);
}

TEST_F(AttractionsParserTest, ParseLine_ValidString_CorrectType) {
    parser.init_parser(valid_header);
    AttractionItem item;
    parser.parse_line(item, valid_line);

    EXPECT_EQ(item.first.type, "museum");
}

TEST_F(AttractionsParserTest, ParseLine_ValidString_CorrectName) {
    parser.init_parser(valid_header);
    AttractionItem item;
    parser.parse_line(item, valid_line);

    EXPECT_EQ(item.first.name, "Kremlin");
}

TEST_F(AttractionsParserTest, ParseLine_ValidString_CorrectExtraFieldsSize) {
    parser.init_parser(valid_header);
    AttractionItem item;
    parser.parse_line(item, valid_line);

    EXPECT_EQ(item.first.extra_fields.size(), 2u);
}

TEST_F(AttractionsParserTest, ParseLine_QuotesString_CorrectName) {
    parser.init_parser(quotes_header);
    AttractionItem item;
    parser.parse_line(item, quotes_line);

    EXPECT_EQ(item.first.name, "Peterhof");
}

TEST_F(AttractionsParserTest, ParseLine_CarriageString_CorrectVisited) {
    parser.init_parser(quotes_header);
    AttractionItem item;
    parser.parse_line(item, carriage_line);

    std::string visited = item.first.extra_fields.count("visited")
                              ? item.first.extra_fields.at("visited")
                              : "";
    EXPECT_EQ(visited, "yes");
}

TEST_F(AttractionsParserTest, ParseLine_EmptyString_ReturnsFalse) {
    parser.init_parser(quotes_header);
    AttractionItem item;

    EXPECT_FALSE(parser.parse_line(item, ""));
}
