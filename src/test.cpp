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

TEST_CASE("detect lower/upper/snake/camel case", "[symbol]") {
    REQUIRE(isCamelCase("thisIsACamelCase") == true);
    REQUIRE(isCamelCase("ThisIsNotACamelCase") == false);
    REQUIRE(isCamelCase("This_Is_Also_Not_A_Camel_Case") == false);

    REQUIRE(isPascalCase("ThisIsPascalCase") == true);
    REQUIRE(isPascalCase("thisIsNotAPascalCase") == false);
    REQUIRE(isPascalCase("THIS_IS_ALSO_NOT_A_PASCAL_CASE") == false);

    REQUIRE(isLowerSnakeCase("this_is_lower_snake_Case") == true);
    REQUIRE(isLowerSnakeCase("thisIsNotALowerSnakeCase") == false);

    REQUIRE(isUpperSnakeCase("THIS_IS_UPPER_SNAKE_CASE") == true);
    REQUIRE(isUpperSnakeCase("ThisIsNotAUpperSnakeCase") == false);
}