// Copyright 2025 itlab-vision
#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "locations_generated.hpp"  // NOLINT

struct LocationInfo {
    unsigned int location_id;
    std::string country;
    std::string city;
    int region_id = -1;
    int district_id = -1;
    std::vector<std::string> alt_names;
    std::string attraction_db_path = "";

    LocationInfo() {}
    LocationInfo(unsigned int id, std::string cntry, std::string cty, int rgn,
                 int dstr, const std::vector<std::string>& names)
        : location_id(id),
          country(cntry),
          city(cty),
          region_id(rgn),
          district_id(dstr),
          alt_names(names) {}
    LocationInfo(unsigned int id, std::string cntry, std::string cty,
                 std::string attr_db_path)
        : location_id(id),
          country(cntry),
          city(cty),
          attraction_db_path(attr_db_path) {}
    LocationInfo(unsigned int id, std::string cntry, std::string cty, int rgn,
                 int dstr, const std::vector<std::string>& names,
                 std::string attr_db_path)
        : location_id(id),
          country(cntry),
          city(cty),
          region_id(rgn),
          district_id(dstr),
          alt_names(names),
          attraction_db_path(attr_db_path) {}

    friend std::ostream& operator<<(std::ostream& out,
                                    const LocationInfo& location) {
        out << "\tCountry: " << location.country << "\n";
        out << "\tCity: " << location.city << "\n";
        out << "\tAlternative names:\n";
        for (int i = 0; i < location.alt_names.size(); i++) {
            out << "\t\t" << location.alt_names[i] << "\n";
        }
        if (!location.attraction_db_path.empty()) {
            out << "\tAttraction DB Path: " << location.attraction_db_path
                << "\n";
        }
        return out;
    }

    LocationInfo& operator=(const LocationsData::LocationInfo* info) {
        this->location_id = info->location_id();
        this->country = info->country()->str();
        this->city = info->city()->str();
        this->region_id = info->region_id();
        this->district_id = info->district_id();
        this->attraction_db_path = info->attraction_db_path()->str();

        this->alt_names.clear();
        auto fb_alt_names = info->alt_names();
        if (fb_alt_names) {
            this->alt_names.reserve(fb_alt_names->size());
            for (auto fb_str : *fb_alt_names) {
                this->alt_names.emplace_back(fb_str->str());
            }
        }

        return *this;
    }

    explicit LocationInfo(const LocationsData::LocationInfo* info) {
        *this = info;
    }
};
