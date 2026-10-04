// Copyright 2025 itlab-vision
#include <gtest/gtest.h>
#include <string>

#include "locations_parser.hpp"  // NOLINT

class LocationsParserTest : public ::testing::Test {
 protected:
    LocationsParser parser;

    const std::string basic_line = "1;Russia;Moscow;55.75;37.61";
    const std::string alt_names_line =
        "2;Russia;SPB;59.93;30.33;Saint-Petersburg,Piter";
    const std::string full_line = "3;Russia;Kazan;55.79;49.10;16;5;KZN";
    const std::string attr_db_line = "4;Russia;Sochi;43.59;39.72;sochi.db";
    const std::string quotes_line =
        "\"5\";\"Russia\";\"Omsk\";\"54.98\";\"73.36\"";
};

TEST_F(LocationsParserTest, ParseLine_BasicString_ReturnsTrue) {
    parser.init_parser("id;country;city;lat;lon");
    LocationItem item;
    EXPECT_TRUE(parser.parse_line(item, basic_line));
}

TEST_F(LocationsParserTest, ParseLine_BasicString_CorrectId) {
    parser.init_parser("id;country;city;lat;lon");
    LocationItem item;
    parser.parse_line(item, basic_line);
    EXPECT_EQ(item.first.location_id, 1u);
}

TEST_F(LocationsParserTest, ParseLine_BasicString_CorrectCountry) {
    parser.init_parser("id;country;city;lat;lon");
    LocationItem item;
    parser.parse_line(item, basic_line);
    EXPECT_EQ(item.first.country, "Russia");
}

TEST_F(LocationsParserTest, ParseLine_BasicString_CorrectLat) {
    parser.init_parser("id;country;city;lat;lon");
    LocationItem item;
    parser.parse_line(item, basic_line);
    EXPECT_DOUBLE_EQ(item.second.latitude, 55.75);
}

TEST_F(LocationsParserTest, ParseLine_AltNamesString_CorrectAltSize) {
    parser.init_parser("id;country;city;lat;lon;alt_names");
    LocationItem item;
    parser.parse_line(item, alt_names_line);
    EXPECT_EQ(item.first.alt_names.size(), 2u);
}

TEST_F(LocationsParserTest, ParseLine_AltNamesString_CorrectFirstAlt) {
    parser.init_parser("id;country;city;lat;lon;alt_names");
    LocationItem item;
    parser.parse_line(item, alt_names_line);
    EXPECT_EQ(item.first.alt_names.empty() ? "" : item.first.alt_names[0],
              "Saint-Petersburg");
}

TEST_F(LocationsParserTest, ParseLine_FullString_CorrectRegionId) {
    parser.init_parser(
        "id;country;city;lat;lon;region_id;district_id;alt_names");
    LocationItem item;
    parser.parse_line(item, full_line);
    EXPECT_EQ(item.first.region_id, 16);
}

TEST_F(LocationsParserTest, ParseLine_FullString_CorrectDistrictId) {
    parser.init_parser(
        "id;country;city;lat;lon;region_id;district_id;alt_names");
    LocationItem item;
    parser.parse_line(item, full_line);
    EXPECT_EQ(item.first.district_id, 5);
}

TEST_F(LocationsParserTest, ParseLine_AttrDbString_CorrectAttrDbPath) {
    parser.init_parser("id;country;city;lat;lon;attraction_db_path");
    LocationItem item;
    parser.parse_line(item, attr_db_line);
    EXPECT_EQ(item.first.attraction_db_path, "sochi.db");
}

TEST_F(LocationsParserTest, ParseLine_QuotesString_CorrectCity) {
    parser.init_parser("id;country;city;lat;lon");
    LocationItem item;
    parser.parse_line(item, quotes_line);
    EXPECT_EQ(item.first.city, "Omsk");
}

TEST_F(LocationsParserTest, ParseLine_EmptyString_ReturnsFalse) {
    parser.init_parser("id;country;city;lat;lon");
    LocationItem item;
    EXPECT_FALSE(parser.parse_line(item, ""));
}
