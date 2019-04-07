#include "util.h"

using namespace bin2h;

std::tuple<std::string, std::string>
    bin2h::getDirectoryAndFilename(const std::string& fullpathFilename) {
    size_t found = fullpathFilename.find_last_of("/\\");
    if (found == std::string::npos) {
        return std::make_tuple("", fullpathFilename);
    }
    return std::make_tuple(
        fullpathFilename.substr(0, found),
        fullpathFilename.substr(found+1)
    );
}

std::tuple<std::string, std::string> bin2h::getNameAndExtension(const std::string& filename) {
    size_t found = filename.find_last_of(".");
    if (found == std::string::npos) {
        return std::make_tuple(filename, "");
    }
    return std::make_tuple(
        filename.substr(0, found),
        filename.substr(found+1)
    );
}

bool bin2h::isCamelCase(const std::string& symbol) {
    return false;
}

bool bin2h::isPascalCase(const std::string& symbol) {
    return false;
}

bool bin2h::isLowerSnakeCase(const std::string& symbol) {
    return false;
}

bool bin2h::isUpperSnakeCase(const std::string& symbol) {
    return false;
}