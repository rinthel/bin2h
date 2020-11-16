#include "util.h"
#include <algorithm>
#include <sstream>
#include <cctype>
#include <fstream>
#include <spdlog/spdlog.h>

namespace bin2h {

std::tuple<std::string, std::string> getDirectoryAndFilename(const std::string& fullpathFilename)
{
    size_t found = fullpathFilename.find_last_of("/\\");
    if (found == std::string::npos)
    {
        return std::make_tuple("", fullpathFilename);
    }
    return std::make_tuple(
        fullpathFilename.substr(0, found),
        fullpathFilename.substr(found + 1)
    );
}

std::tuple<std::string, std::string> getNameAndExtension(const std::string& filename)
{
    size_t found = filename.find_last_of(".");
    if (found == std::string::npos)
    {
        return std::make_tuple(filename, "");
    }
    return std::make_tuple(
        filename.substr(0, found),
        filename.substr(found + 1)
    );
}

inline bool isNumber(char c) { return (c >= '0' && c <= '9'); }
inline bool isLowerCase(char c) { return (c >= 'a' && c <= 'z'); }
inline bool isUpperCase(char c) { return (c >= 'A' && c <= 'Z'); }
inline bool isCharacter(char c) { return isNumber(c) || isUpperCase(c) || isLowerCase(c); }

bool hasOnlyAlphabetAndNumber(const std::string& symbol)
{
    if (symbol.size() == 0)
        return false;
    for (auto& c: symbol)
    {
        if (!isCharacter(c))
            return false;
    }
    return true;
}

bool isCamelCase(const std::string& symbol)
{
    return hasOnlyAlphabetAndNumber(symbol) && isLowerCase(symbol[0]);
}

bool isPascalCase(const std::string& symbol)
{
    return hasOnlyAlphabetAndNumber(symbol) && isUpperCase(symbol[0]);
}

bool isLowerSnakeCase(const std::string& symbol)
{
    if (symbol.size() == 0)
        return false;
    for (auto& c: symbol)
    {
        if (!(isLowerCase(c) || c == '_'))
            return false;
    }
    return true;
}

bool isUpperSnakeCase(const std::string& symbol)
{
    if (symbol.size() == 0)
        return false;
    for (auto& c: symbol)
    {
        if (!(isUpperCase(c) || c == '_'))
            return false;
    }
    return true;
}

std::vector<std::string> splitSymbol(const std::string& symbol, Delimiter delimiter)
{
    std::vector<std::string> splitResult;
    if (symbol.size() == 0)
        return splitResult;
    if (symbol.size() == 1)
    {
        splitResult.push_back(symbol);
        return splitResult;
    }
    size_t prevPos = 0;
    for (size_t i = 1; i < symbol.size(); i++)
    {
        char c = symbol[i];
        if ((delimiter == Delimiter::upperCase && isUpperCase(c)) ||
            (delimiter == Delimiter::underscore && c == '_') ||
            !isCharacter(c))
        {
            auto token = symbol.substr(prevPos, i - prevPos);
            if (!token.empty() && token.find('_') == std::string::npos)
                splitResult.push_back(token);
            prevPos = i + (delimiter == Delimiter::underscore ? 1 : 0);
        }
    }
    if (prevPos < symbol.size()) {
        auto token = symbol.substr(prevPos);
        if (!token.empty())
            splitResult.push_back(token);
    }
    return splitResult;
}

std::string convertSymbol(const std::string& symbol, CaseNotation notation)
{
    auto convertLower = [](std::string& token) -> std::string& {
        for (auto& c : token) {
            c = isUpperCase(c) ? std::tolower(c) : c;
        }
        return token;
    };
    auto convertUpper = [](std::string& token) -> std::string& {
        for (auto& c : token) {
            c = isLowerCase(c) ? std::toupper(c) : c;
        }
        return token;
    };
    auto convertPascal = [&](std::string& token) {
        convertLower(token);
        if (token.size() > 0)
            token[0] = std::toupper(token[0]);
        return token;
    };
    auto tokens = splitSymbol(symbol, (hasOnlyAlphabetAndNumber(symbol) ? Delimiter::upperCase : Delimiter::underscore));
    std::stringstream sout;
    for (size_t i = 0; i < tokens.size(); ++i) {
        switch (notation) {
            case CaseNotation::camelCase:
                sout << (i == 0 ? convertLower(tokens[i]) : convertPascal(tokens[i]));
                break;
            case CaseNotation::pascalCase:
                sout << convertPascal(tokens[i]);
                break;
            case CaseNotation::lowerSnakeCase:
                sout << convertLower(tokens[i]) << (i < tokens.size() - 1 ? "_" : "");
                break;
            case CaseNotation::upperSnakeCase:
                sout << convertUpper(tokens[i]) << (i < tokens.size() - 1 ? "_" : "");
                break;
        }
    }
    return sout.str();
}

InputData::InputData(const std::string _inputFilename, bool includeExtension)
: inputFilename(_inputFilename)
{
    spdlog::info("open input file: {}", inputFilename);
    std::ifstream in(inputFilename, std::ifstream::binary);
    if (!in.is_open()) {
        throw std::ios_base::failure(fmt::format("failed to open file \"{}\"", inputFilename));
    }
    in.seekg(0, in.end);
    filesize = (size_t)in.tellg();
    data.resize(filesize);
    memset(data.data(), 255, data.size());
    in.seekg(0, in.beg);

    in.read(reinterpret_cast<char*>(data.data()), filesize);
    spdlog::info("read data of file {}: {} bytes", inputFilename, data.size());
    
    in.close();

    std::string inputDirectoryPath, inputFilePath, inputName, inputExtension;
    std::tie(inputDirectoryPath, inputFilePath) = bin2h::getDirectoryAndFilename(inputFilename);
    std::tie(inputName, inputExtension) = bin2h::getNameAndExtension(inputFilePath);

    arrayName = bin2h::convertSymbol(
        includeExtension ? inputFilePath : inputName,
        bin2h::CaseNotation::upperSnakeCase);
    sizeName = arrayName + "_SIZE";
}

std::pair<uint64_t, uint64_t> computeMD5(const std::vector<std::pair<void*, size_t>> messages)
{
	uint8_t digest[PICOHASH_MD5_DIGEST_LENGTH];
	picohash_ctx_t context;
	picohash_init_md5(&context);
    for (auto&& message: messages) {
    	picohash_update(&context, message.first, message.second);
    }
	picohash_final(&context, digest);
	auto digest64 = reinterpret_cast<uint64_t*>(digest);
	return {digest64[0], digest64[1]};
}

} // delimiter