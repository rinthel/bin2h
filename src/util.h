#ifndef BIN2H_UTIL_H
#define BIN2H_UTIL_H

#include <string>
#include <tuple>
#include <vector>

#include <picohash.h>

namespace bin2h {

std::tuple<std::string, std::string> getDirectoryAndFilename(const std::string& fullpathFilename);
std::tuple<std::string, std::string> getNameAndExtension(const std::string& filename);
bool isCamelCase(const std::string& symbol);
bool isPascalCase(const std::string& symbol);
bool isLowerSnakeCase(const std::string& symbol);
bool isUpperSnakeCase(const std::string& symbol);

enum Delimiter
{
    upperCase,
    underscore,
};

enum CaseNotation
{
    camelCase,
    pascalCase,
    lowerSnakeCase,
    upperSnakeCase,
};

std::vector<std::string> splitSymbol(const std::string& symbol, Delimiter delimiter);
std::string convertSymbol(const std::string& symbol, CaseNotation notation);

struct InputData {
    std::string inputFilename;

    std::string arrayName;
    std::string sizeName;
    std::vector<uint8_t> data;
    size_t filesize;

    InputData(const std::string _inputFilename, bool includeExtension);
};

using MD5 = std::pair<uint64_t, uint64_t>;
using Message = std::pair<void*, size_t>;
MD5 computeMD5(const std::vector<Message> messages);

} // bin2h

#endif // BIN2H_UTIL_H