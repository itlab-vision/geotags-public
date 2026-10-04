// Copyright 2025 itlab-vision
#pragma once

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <stdexcept>

#include "reader_interface.hpp"       // NOLINT
#include "flatbuffers/flatbuffers.h"  // NOLINT

template <typename Parser, typename Converter, typename WrapperType>
class BaseGenericReaderFB : public ReaderInterface<WrapperType> {
 public:
    WrapperType read() override {
        this->prepare_file();
        return this->deserialize();
    }

 protected:
    Parser parser_{};
    Converter converter_{};

    explicit BaseGenericReaderFB(const std::string& fname)
        : ReaderInterface<WrapperType>(fname) {}
    virtual ~BaseGenericReaderFB() = default;

    BaseGenericReaderFB(const BaseGenericReaderFB&) = delete;
    BaseGenericReaderFB& operator=(const BaseGenericReaderFB&) = delete;
    BaseGenericReaderFB(BaseGenericReaderFB&&) = delete;
    BaseGenericReaderFB& operator=(BaseGenericReaderFB&&) = delete;

    static bool file_exists(const std::string& path) {
        struct stat buffer;
        return stat(path.c_str(), &buffer) == 0;
    }
    static time_t file_mtime(const std::string& path) {
        struct stat buffer;
        if (stat(path.c_str(), &buffer) == 0) return buffer.st_mtime;
        return 0;
    }
    void prepare_file();

    std::shared_ptr<char> read_buffer();

    void serialize();
    virtual WrapperType deserialize() = 0;
};

template <typename Parser, typename Converter, typename WrapperType>
void BaseGenericReaderFB<Parser, Converter, WrapperType>::prepare_file() {
    std::string fb_file = this->file_name + ".fb";
    if (!file_exists(fb_file) ||
        file_mtime(this->file_name) > file_mtime(fb_file)) {
        serialize();
    }
}

template <typename Parser, typename Converter, typename WrapperType>
std::shared_ptr<char>
BaseGenericReaderFB<Parser, Converter, WrapperType>::read_buffer() {
    std::string fb_file = this->file_name + ".fb";
    int fd = ::open(fb_file.c_str(), O_RDONLY);
    if (fd == -1) {
        throw std::runtime_error("Can't open FlatBuffer file: " + fb_file);
    }

    struct stat sb;
    if (::fstat(fd, &sb) == -1) {
        ::close(fd);
        throw std::runtime_error("Can't get file size: " + fb_file);
    }
    size_t size = sb.st_size;

    // NOTE: MAP_POPULATE forces full file loading into RAM.
    // This slows down the initial mmap call but guarantees maximum access speed
    // later by preventing page faults.
    void* mapped =
        ::mmap(nullptr, size, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0);
    ::close(fd);

    if (mapped == MAP_FAILED) {
        throw std::runtime_error("mmap failed for file: " + fb_file);
    }

    return std::shared_ptr<char>(static_cast<char*>(mapped),
                                 [size](char* p) { ::munmap(p, size); });
}

template <typename Parser, typename Converter, typename WrapperType>
void BaseGenericReaderFB<Parser, Converter, WrapperType>::serialize() {
    std::string items_size;
    std::getline(this->file, items_size);

    std::string attributes;
    std::getline(this->file, attributes);
    parser_.init_parser(attributes);

    flatbuffers::FlatBufferBuilder builder(1024 * 1024);

    // NOTE: ForceDefaults(true) ensures a fixed memory layout for all tables.
    // This allows O(1) field access via raw pointer arithmetic in wrapper
    // classes.

    std::vector<decltype(converter_.convert_to_fb(
        builder, typename Converter::ItemType{}))>
        fb_items;
    fb_items.reserve(parser_.template convert_num_argument<size_t>(items_size));

    typename Converter::ItemType item{};
    while (!this->file.eof()) {
        this->line_buffer.clear();
        std::getline(this->file, this->line_buffer);

        if (parser_.parse_line(item, this->line_buffer)) {
            fb_items.push_back(converter_.convert_to_fb(builder, item));
        }
    }
    auto data_vec = builder.CreateVector(fb_items);
    auto root_fb = converter_.create_root_fb(builder, data_vec);
    builder.Finish(root_fb);

    std::string fb_file = this->file_name + ".fb";
    std::ofstream fout(fb_file, std::ios::binary);
    fout.write(reinterpret_cast<const char*>(builder.GetBufferPointer()),
               builder.GetSize());
}
