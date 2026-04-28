// Copyright 2025 itlab-vision
#pragma once

#include <iostream>
#include <string>
#include <vector>

struct LocationInfo {
    unsigned int location_id;
    std::string country;
    std::string city;
    int region_id = -1;
    int district_id = -1;
    std::vector<std::string> alt_names;

    LocationInfo() {}
    LocationInfo(std::string cntry, std::string cty,
                 const std::vector<std::string> &names)
        : country(cntry), city(cty), region_id{}, alt_names(names) {}
    LocationInfo(std::string cntry, std::string cty, unsigned int rgn,
                 const std::vector<std::string> &names)
        : country(cntry), city(cty), region_id(rgn), alt_names(names) {}
    LocationInfo(unsigned int id, std::string cntry, std::string cty, int rgn,
                 const std::vector<std::string> &names)
        : location_id(id),
          country(cntry),
          city(cty),
          region_id(rgn),
          alt_names(names) {}
    LocationInfo(unsigned int id, std::string cntry, std::string cty, int rgn,
                 int dstr, const std::vector<std::string> &names)
        : location_id(id),
          country(cntry),
          city(cty),
          region_id(rgn),
          district_id(dstr),
          alt_names(names) {}

    friend std::ostream &operator<<(std::ostream &out,
                                    const LocationInfo &location) {
        out << "\tCountry: " << location.country << "\n";
        out << "\tCity: " << location.city << "\n";
        out << "\tAlternative names:\n";
        for (int i = 0; i < location.alt_names.size(); i++) {
            out << "\t\t" << location.alt_names[i] << "\n";
        }
        return out;
    }
};
