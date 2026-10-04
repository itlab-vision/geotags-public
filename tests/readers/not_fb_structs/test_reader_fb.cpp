// // Copyright 2025 itlab-vision
// #include <gtest/gtest.h>
// #include <fstream>
// #include <cstdio>
// #include <string>

// #include "reader.hpp"  // NOLINT

// class TestableReader : public Reader {
//  public:
//     explicit TestableReader(const std::string& fname) : Reader(fname) {}
//     using Reader::file_exists;
//     using Reader::file_mtime;
// };

// class ReaderFBStaticTest : public ::testing::Test {
//  protected:
//     void SetUp() override {
//         fname_exists = "temp_reader_fb_exists.txt";
//         std::ofstream f(fname_exists);
//         f << "data";
//         f.close();

//         fname_nonexistent = "temp_reader_fb_nonexistent.txt";
//         std::remove(fname_nonexistent.c_str());
//     }

//     void TearDown() override { std::remove(fname_exists.c_str()); }

//     std::string fname_exists;
//     std::string fname_nonexistent;
// };

// TEST_F(ReaderFBStaticTest, FileExistsReturnsTrueForExistingFile) {
//     EXPECT_TRUE(TestableReader::file_exists(fname_exists));
// }

// TEST_F(ReaderFBStaticTest, FileMTimeReturnsNonZeroForExistingFile) {
//     EXPECT_GT(TestableReader::file_mtime(fname_exists), 0);
// }

// TEST_F(ReaderFBStaticTest, FileExistsReturnsFalseForNonexistent) {
//     EXPECT_FALSE(TestableReader::file_exists(fname_nonexistent));
// }

// TEST_F(ReaderFBStaticTest, FileMTimeReturnsZeroForNonexistent) {
//     EXPECT_EQ(TestableReader::file_mtime(fname_nonexistent), 0);
// }
