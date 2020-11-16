#ifdef WIN32
#define NOMINMAX
#endif
#include <iostream>
#include <vector>
#include <fstream>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>
#include <clipp.h>

#include "util.h"

#ifndef BIN2H_VERSION
#define BIN2H_VERSION "v0.1.0"
#endif

int main(int argc, char** argv) {
    bool help = false;
    bool verboseMode = false;
    bool comment = false;
    bool includeExtension = false;
    bool force = false;
    size_t align = 1;
    size_t bytePerLine = 8;
    std::vector<std::string> inputFilenames;
    std::string outputFilename;
    auto cli = (
        clipp::values("input binary files").required(true).set(inputFilenames),
        (clipp::required("-o", "--output") & clipp::value("output header file").set(outputFilename)),
        clipp::option("-h", "--help").set(help, true) % "show this help text",
        clipp::option("-v", "--verbose").set(verboseMode, true) % "print verbose log",
        clipp::option("-c", "--comment").set(comment, true) % "write ascii output of binaries at the end of each line",
        (clipp::option("-a", "--align") & clipp::value("align byte size").set(align)) % "set align byte size of each array",
        (clipp::option("-l", "--line") & clipp::value("byte count per line").set(bytePerLine)) % "set byte count per line",
        (clipp::option("-e", "--include-extension").set(includeExtension)) % "include extension for naming array variables",
        (clipp::option("-f", "--force").set(force)) % "force to generate output, although input does not change"
    );

    auto parseResult = clipp::parse(argc, argv, cli);
    if (!parseResult || help) {
        std::cout << clipp::make_man_page(cli, argv[0]);
        return -1;
    }

    spdlog::set_level(verboseMode ? spdlog::level::info : spdlog::level::err);

    // sort input filenames to alphabetical order
    std::sort(inputFilenames.begin(), inputFilenames.end());
    for (size_t i = 0; i < inputFilenames.size(); i++) {
        spdlog::info("input filename[{}]: {}", i, inputFilenames[i]);
    }
    spdlog::info("output filename: {}", outputFilename);

    std::vector<std::shared_ptr<bin2h::InputData>> inputDataList;
    bin2h::MD5 md5;
    inputDataList.reserve(inputFilenames.size());
    try {
        spdlog::info("open input files and load binaries");
        for (auto& inputFilename: inputFilenames) {
            inputDataList.push_back(std::make_shared<bin2h::InputData>(inputFilename, includeExtension));
        }
        std::vector<bin2h::Message> messages;
        messages.reserve(inputDataList.size());
        for (auto& inputData: inputDataList) {
            messages.push_back(std::make_pair(
                reinterpret_cast<void*>(inputData->data.data()),
                inputData->data.size()));
        }
        md5 = bin2h::computeMD5(messages);
        spdlog::info("input data MD5 checksum: {}", fmt::format("{:016x}{:016x}", md5.first, md5.second));
    }
    catch (std::exception& e) {
        spdlog::error("failed to process input files: {}", e.what());
        return -1;
    }

    // check that output file exists,
    std::ifstream fin(outputFilename);
    if (fin.is_open()) {
        char firstLine[512], md5String[64];
        fin.getline(firstLine, 512);
        sscanf(firstLine, "//! MD5 checksum: %s", md5String);
        fin.close();

        bin2h::MD5 outputMd5;
        auto convertHexString = [](const char* str) {
            uint64_t v = 0;
            for (size_t i = 0; i < 16; i++) {
                size_t shift = 15 - i;
                uint8_t c = str[i];
                uint8_t hex = (
                    (c >= '0' && c <= '9') ? c - '0' :
                    (c >= 'a' && c <= 'f') ? c - 'a' + 10 :
                    c - 'A' + 10
                    );
                v |= static_cast<uint64_t>(hex) << (shift * 4);
            }
            return v;
        };
        outputMd5.first = convertHexString(md5String);
        outputMd5.second = convertHexString(md5String + 16);

        spdlog::info("found output file's MD5: {:016x}{:016x}", outputMd5.first, outputMd5.second);
        if (outputMd5 == md5) {
            spdlog::info("output's MD5 checksum is equal to input");
            if (!force) {
                spdlog::info("stop generating.");
                exit(0);
            }
            else {
                spdlog::info("find forced output option. proceed to generate new output file");
            }
        }
    }
    else {
        spdlog::info("cannot find output file, generate new one");
    }

    try {
        spdlog::info("open output file: {}", outputFilename);
        std::ofstream out(outputFilename);

        std::string outputDirectoryPath, outputFilePath, outputName, outputExtension;
        std::tie(outputDirectoryPath, outputFilePath) = bin2h::getDirectoryAndFilename(outputFilename);
        std::tie(outputName, outputExtension) = bin2h::getNameAndExtension(outputFilePath);
        std::string predefinedSymbol = bin2h::convertSymbol(outputName, bin2h::CaseNotation::upperSnakeCase) + "_H";

        out << fmt::format("//! MD5 checksum: {:016x}{:016x}", md5.first, md5.second) << std::endl;
        out << fmt::format("//! automatically generated header file via bin2h (https://github.com/rinthel/bin2h).") << std::endl;
        out << fmt::format("//! Please do not modify.") << std::endl;
        out << fmt::format("#ifndef {}", predefinedSymbol) << std::endl;
        out << fmt::format("#define {}", predefinedSymbol) << std::endl;
        out << std::endl;

        for (auto&& inputData: inputDataList) {
            spdlog::info("write input file: {} -> {}", inputData->inputFilename, inputData->arrayName);

            std::stringstream commentString;
            size_t alignedSize = (static_cast<size_t>(inputData->filesize) + align - 1) / align * align;
            out << fmt::format("static const unsigned long {} = {};", inputData->sizeName, alignedSize) << std::endl;
            out << fmt::format("static const unsigned char {}[{}] = ", inputData->arrayName, inputData->sizeName) << '{' << std::endl;

            for (size_t i = 0; i < alignedSize; ++i) {
                if (i % bytePerLine == 0) {
                    out << "    ";
                    commentString.str(std::string());
                    commentString << "// ";
                }
                uint8_t elem = (i < inputData->data.size() ? inputData->data[i] : 0);
                out << fmt::format("0x{:02x}, ", elem);
                commentString << ((elem >= 32 && elem < 127 && elem != '\\') ? static_cast<char>(elem) : '.');
                if (i % bytePerLine == bytePerLine - 1) {
                    if (comment) {
                        out << commentString.str();
                    }
                    out << std::endl;
                }
            }
            if (alignedSize % bytePerLine != 0) {
                if (comment) {
                    out << commentString.str();
                }
                out << std::endl;
            }
            out << "};" << std::endl << std::endl;
        }

        out << fmt::format("#endif // {}", predefinedSymbol) << std::endl;
    }
    catch (std::exception& e) {
        spdlog::error("failed to process: {}", e.what());
        return -1;
    }

    spdlog::info("successfully generated: {}", outputFilename);

    return 0;
}