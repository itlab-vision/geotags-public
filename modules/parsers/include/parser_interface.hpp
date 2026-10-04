// Copyright 2025 itlab-vision
#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <charconv>
#include <type_traits>

#include "flatbuffers/flatbuffers.h"  // NOLINT

template <typename ItemType>
class ParserInterface {
 private:
    // separate_cells buffer
    std::string cell_buffer;
    std::vector<std::string> row_buffer;

    // convert_vec_argument buffer
    std::string vec_elem_buffer;

 public:
    virtual void init_parser(const std::string& header) = 0;
    virtual bool parse_line(ItemType& item, const std::string& str) = 0;

    template <typename T>
    T convert_num_argument(const std::string& str);

 protected:
    void remove_quotes(std::string& str) {
        str.erase(std::remove(str.begin(), str.end(), '\"'), str.end());
    }
    void remove_carriages(std::string& str) {
        str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    }
    const std::vector<std::string>& separate_cells(const std::string& str);

    explicit ParserInterface() {
        cell_buffer.reserve(2048);
        row_buffer.reserve(16);
        vec_elem_buffer.reserve(256);
    }

    template <typename T>
    T try_convert_num_argument(const std::string& str, T& value);

    template <typename T>
    std::vector<T> convert_vec_argument(const std::string& str,
                                        char delimiter = ',');

    virtual ~ParserInterface() = default;

    ParserInterface(const ParserInterface&) = delete;
    ParserInterface& operator=(const ParserInterface&) = delete;
    ParserInterface(ParserInterface&&) = delete;
    ParserInterface& operator=(ParserInterface&&) = delete;
};

template <typename ItemType>
const std::vector<std::string>& ParserInterface<ItemType>::separate_cells(
    const std::string& str) {
    if (str.empty()) {
        row_buffer.clear();
        return row_buffer;
    }

    size_t start = 0;
    size_t end = str.find(';');
    size_t col_idx = 0;

    auto add_cell = [&](size_t c_start, size_t c_count) {
        if (col_idx >= row_buffer.size()) {
            row_buffer.emplace_back();
        }
        row_buffer[col_idx].assign(str, c_start, c_count);
        remove_quotes(row_buffer[col_idx]);
        remove_carriages(row_buffer[col_idx]);
        col_idx++;
    };

    while (end != std::string::npos) {
        add_cell(start, end - start);
        start = end + 1;
        end = str.find(';', start);
    }

    add_cell(start, std::string::npos);

    row_buffer.resize(col_idx);

    return row_buffer;
}

template <typename ItemType>
template <typename T>
T ParserInterface<ItemType>::convert_num_argument(const std::string& str) {
    T value{};
    if (str.empty()) {
        return value;
    }

    if constexpr (std::is_floating_point_v<T>) {
        value = static_cast<T>(std::stod(str));
    } else {
        std::from_chars(str.data(), str.data() + str.size(), value);
    }

    return value;
}

template <typename ItemType>
template <typename T>
T ParserInterface<ItemType>::try_convert_num_argument(const std::string& str,
                                                      T& value) {
    T argument{};
    std::istringstream ss(str);

    std::string remained;
    if (!(ss >> argument) || ss >> remained) {
        return false;
    }

    value = argument;
    return true;
}

template <typename ItemType>
template <typename T>
std::vector<T> ParserInterface<ItemType>::convert_vec_argument(
    const std::string& str, char delimiter) {
    if (str.empty()) {
        return {};
    }

    std::vector<T> result;
    result.reserve(16);

    std::string_view sv(str);
    size_t start = 0;
    size_t end = sv.find(delimiter);

    auto process_token = [&](size_t t_start, size_t t_count) {
        std::string_view token = sv.substr(t_start, t_count);

        if constexpr (std::is_same_v<T, std::string>) {
            result.emplace_back(token);
        } else {
            vec_elem_buffer.assign(token.data(), token.size());
            result.emplace_back(convert_num_argument<T>(vec_elem_buffer));
        }
    };

    while (end != std::string_view::npos) {
        process_token(start, end - start);
        start = end + 1;
        end = sv.find(delimiter, start);
    }

    process_token(start, std::string_view::npos);

    return result;
}
