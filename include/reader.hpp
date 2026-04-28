// Copyright 2025 itlab-vision
#pragma once

#include <sys/stat.h>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <fstream>
#include <ctime>

class Reader {
 private:
    // separate_cells buffer
    std::string line_buffer;
    std::string cell_buffer;
    std::vector<std::string> row_buffer;

    // convert_vec_argument buffer
    std::string vec_elem_buffer;

 protected:
    std::string file_name;
    std::ifstream file;

    explicit Reader(const std::string& fname);

    void open_file();

    void remove_quotes(std::string& str);
    void remove_carriages(std::string& str);

    static bool file_exists(const std::string& path);
    static time_t file_mtime(const std::string& path);

    std::vector<std::string> separate_cells();

    template <typename T>
    T convert_num_argument(const std::string& str);

    template <typename T>
    T try_convert_num_argument(const std::string& str, T& value);

    template <typename T>
    std::vector<T> convert_vec_argument(const std::string& str);
};

template <typename T>
T Reader::convert_num_argument(const std::string& str) {
    T argument;
    std::istringstream ss(str);
    ss >> argument;
    return argument;
}

// TODO(TepidmishA): deprecate this method for better performance
template <typename T>
T Reader::try_convert_num_argument(const std::string& str, T& value) {
    T argument;
    std::istringstream ss(str);

    std::string remained;
    if (!(ss >> argument) || ss >> remained) {
        return false;
    }

    value = argument;
    return true;
}

template <typename T>
std::vector<T> Reader::convert_vec_argument(const std::string& str) {
    std::vector<T> result;
    result.reserve(16);

    std::stringstream ss(str);

    while (std::getline(ss, vec_elem_buffer, ',')) {
        std::stringstream token_stream(vec_elem_buffer);
        T value;
        token_stream >> value;
        result.push_back(value);
    }

    return result;
}
