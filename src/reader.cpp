// Copyright 2025 itlab-vision

#include <vector>
#include <string>

#include "reader.hpp"  // NOLINT

Reader::Reader(const std::string& fname) : file_name(fname) {
    line_buffer.reserve(4096);
    cell_buffer.reserve(2048);
    row_buffer.reserve(16);

    vec_elem_buffer.reserve(256);
}

void Reader::open_file() {
    file = std::ifstream(file_name);
    if (!file) {
        throw std::invalid_argument("Can't open file: " + file_name);
    }
}

void Reader::remove_quotes(std::string& str) {
    str.erase(std::remove(str.begin(), str.end(), '\"'), str.end());
}

void Reader::remove_carriages(std::string& str) {
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
}

bool Reader::file_exists(const std::string& path) {
    struct stat buffer;
    return stat(path.c_str(), &buffer) == 0;
}

time_t Reader::file_mtime(const std::string& path) {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0) return buffer.st_mtime;
    return 0;
}

std::vector<std::string> Reader::separate_cells() {
    line_buffer.clear();
    row_buffer.clear();

    if (!std::getline(file, line_buffer) || line_buffer.empty()) {
        return {};
    }

    std::istringstream lineStream(line_buffer);

    while (std::getline(lineStream, cell_buffer, ';')) {
        remove_quotes(cell_buffer);
        remove_carriages(cell_buffer);
        row_buffer.push_back(cell_buffer);
    }

    if (!lineStream && cell_buffer.empty()) {
        row_buffer.push_back("");
    }

    return row_buffer;
}
