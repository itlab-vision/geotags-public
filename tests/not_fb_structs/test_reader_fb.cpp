// Copyright 2025 itlab-vision

#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include <string>

#include "locations_reader.hpp"  // NOLINT

class TestReaderFB : public LocationsReaderFB {
 public:
    explicit TestReaderFB(const std::string &fname)
        : LocationsReaderFB(fname) {}

    static bool fileExists(const std::string &path) {
        return LocationsReaderFB::file_exists(path);
    }

    static time_t fileMTime(const std::string &path) {
        return LocationsReaderFB::file_mtime(path);
    }
};

TEST(ReaderFBTest, FileExists_ReturnsTrueForExistingFile) {
    std::string fname = "temp_reader_fb_exists.txt";
    std::ofstream f(fname);
    f << "data";
    f.close();

    EXPECT_TRUE(TestReaderFB::fileExists(fname));

    std::remove(fname.c_str());
}

TEST(ReaderFBTest, FileMTime_ReturnsNonZeroForExistingFile) {
    std::string fname = "temp_reader_fb_mtime.txt";
    std::ofstream f(fname);
    f << "data";
    f.close();

    EXPECT_GT(TestReaderFB::fileMTime(fname), 0);

    std::remove(fname.c_str());
}

TEST(ReaderFBTest, FileExists_ReturnsFalseForNonexistent) {
    std::string fname = "temp_reader_fb_nonexistent.txt";
    std::remove(fname.c_str());

    EXPECT_FALSE(TestReaderFB::fileExists(fname));
}

TEST(ReaderFBTest, FileMTime_ReturnsZeroForNonexistent) {
    std::string fname = "temp_reader_fb_nonexistent2.txt";
    std::remove(fname.c_str());

    EXPECT_EQ(TestReaderFB::fileMTime(fname), 0);
}
