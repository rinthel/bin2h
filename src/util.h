#ifndef BIN2H_UTIL_H
#define BIN2H_UTIL_H

#include <tuple>
#include <string>

namespace bin2h {
std::tuple<std::string, std::string> getDirectoryAndFilename(const std::string& fullpathFilename);
std::tuple<std::string, std::string> getNameAndExtension(const std::string& filename);
} // bin2h

#endif BIN2H_UTIL_H