#ifndef BIN2H_UTIL_H
#define BIN2H_UTIL_H

#include <string>
#include <tuple>
#include <vector>

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

} // bin2h

#endif // BIN2H_UTIL_H