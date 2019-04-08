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

    REQUIRE(isLowerSnakeCase("this_is_lower_snake_case") == true);
    REQUIRE(isLowerSnakeCase("thisIsNotALowerSnakeCase") == false);

    REQUIRE(isUpperSnakeCase("THIS_IS_UPPER_SNAKE_CASE") == true);
    REQUIRE(isUpperSnakeCase("ThisIsNotAUpperSnakeCase") == false);
}

TEST_CASE("split symbol with delimiter", "[symbol]") {
    std::vector<std::string> tokens;
    tokens = splitSymbol("camelCaseExample", Delimiter::upperCase);
    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0] == "camel");
    REQUIRE(tokens[1] == "Case");
    REQUIRE(tokens[2] == "Example");

    tokens = splitSymbol("PascalCaseExampleA", Delimiter::upperCase);
    REQUIRE(tokens.size() == 4);
    REQUIRE(tokens[0] == "Pascal");
    REQUIRE(tokens[1] == "Case");
    REQUIRE(tokens[2] == "Example");
    REQUIRE(tokens[3] == "A");

    tokens = splitSymbol("lower_snake_case__example__", Delimiter::underscore);
    REQUIRE(tokens.size() == 4);
    REQUIRE(tokens[0] == "lower");
    REQUIRE(tokens[1] == "snake");
    REQUIRE(tokens[2] == "case");
    REQUIRE(tokens[3] == "example");

    tokens = splitSymbol("__UPPER_SNAKE_CASE__", Delimiter::underscore);
    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0] == "UPPER");
    REQUIRE(tokens[1] == "SNAKE");
    REQUIRE(tokens[2] == "CASE");
}

TEST_CASE("convert symbol into specific case notation", "[symbol]") {
    REQUIRE(convertSymbol("thisIsTestString", CaseNotation::camelCase) == "thisIsTestString");
    REQUIRE(convertSymbol("ThisIsTestString", CaseNotation::camelCase) == "thisIsTestString");
    REQUIRE(convertSymbol("this_is_test_string", CaseNotation::camelCase) == "thisIsTestString");
    REQUIRE(convertSymbol("THIS_IS_TEST_STRING", CaseNotation::camelCase) == "thisIsTestString");

    REQUIRE(convertSymbol("thisIsTestString", CaseNotation::pascalCase) == "ThisIsTestString");
    REQUIRE(convertSymbol("ThisIsTestString", CaseNotation::pascalCase) == "ThisIsTestString");
    REQUIRE(convertSymbol("this_is_test_string", CaseNotation::pascalCase) == "ThisIsTestString");
    REQUIRE(convertSymbol("THIS_IS_TEST_STRING", CaseNotation::pascalCase) == "ThisIsTestString");

    REQUIRE(convertSymbol("thisIsTestString", CaseNotation::lowerSnakeCase) == "this_is_test_string");
    REQUIRE(convertSymbol("ThisIsTestString", CaseNotation::lowerSnakeCase) == "this_is_test_string");
    REQUIRE(convertSymbol("this_is_test_string", CaseNotation::lowerSnakeCase) == "this_is_test_string");
    REQUIRE(convertSymbol("THIS_IS_TEST_STRING", CaseNotation::lowerSnakeCase) == "this_is_test_string");

    REQUIRE(convertSymbol("thisIsTestString", CaseNotation::upperSnakeCase) == "THIS_IS_TEST_STRING");
    REQUIRE(convertSymbol("ThisIsTestString", CaseNotation::upperSnakeCase) == "THIS_IS_TEST_STRING");
    REQUIRE(convertSymbol("this_is_test_string", CaseNotation::upperSnakeCase) == "THIS_IS_TEST_STRING");
    REQUIRE(convertSymbol("THIS_IS_TEST_STRING", CaseNotation::upperSnakeCase) == "THIS_IS_TEST_STRING");
}