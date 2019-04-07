#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "util.h"
using namespace bin2h;

TEST_CASE("get a directory and filename from a fullpath filename", "[filename]") {
    std::string fullpathFilename, dir, filename;

    fullpathFilename = "/Users/myname/Documents/image.png";
    std::tie(dir, filename) = getDirectoryAndFilename(fullpathFilename);
    REQUIRE(dir == "/Users/myname/Documents");
    REQUIRE(filename == "image.png");

    fullpathFilename = "C:\\Users\\myname\\Documents\\image.png";
    std::tie(dir, filename) = getDirectoryAndFilename(fullpathFilename);
    REQUIRE(dir == "C:\\Users\\myname\\Documents");
    REQUIRE(filename == "image.png");

    fullpathFilename = "image.png";
    std::tie(dir, filename) = getDirectoryAndFilename(fullpathFilename);
    REQUIRE(dir.empty());
    REQUIRE(filename == "image.png");
}

TEST_CASE("get a name and extension from a filename", "[filename]") {
    std::string filename, name, extension;

    filename = "image.png";
    std::tie(name, extension) = getNameAndExtension(filename);
    REQUIRE(name == "image");
    REQUIRE(extension == "png");

    filename = "file_name_without_ext";
    std::tie(name, extension) = getNameAndExtension(filename);
    REQUIRE(name == "file_name_without_ext");
    REQUIRE(extension.empty());
}
