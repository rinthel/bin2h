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
#define BIN2H_VERSION "v0.0.0"
#endif

int main(int argc, char** argv) {
    bool help = false;
    bool verboseMode = false;
    bool comment = false;
    size_t align = 1;
    size_t bytePerLine = 8;
    std::vector<std::string> inputFilenames;
    std::string outputFilename;
    auto cli = (
        clipp::values("input binary files").required(true).set(inputFilenames),
        (clipp::required("-o", "--output") & clipp::value("output header file").set(outputFilename)),
        clipp::option("-h", "--help").set(help, true),
        clipp::option("-v", "--verbose").set(verboseMode, true),
        clipp::option("-c", "--comment").set(comment, true),
        (clipp::option("-a", "--align") & clipp::value("align byte size").set(align)),
        (clipp::option("-l", "--line") & clipp::value("byte count per line").set(bytePerLine))
    );

    auto parseResult = clipp::parse(argc, argv, cli);
    if (!parseResult || help) {
        std::cout << clipp::make_man_page(cli, argv[0]);
        return -1;
    }

    spdlog::set_level(verboseMode ? spdlog::level::info : spdlog::level::err);

    for (size_t i = 0; i < inputFilenames.size(); i++) {
        spdlog::info("intput filename[{}]: {}", i, inputFilenames[i]);
    }
    spdlog::info("output filename: {}", outputFilename);

    try {
        spdlog::info("open output file: {}", outputFilename);
        std::ofstream out(outputFilename);

        std::string outputDirectoryPath, outputFilePath, outputName, outputExtension;
        std::tie(outputDirectoryPath, outputFilePath) = bin2h::getDirectoryAndFilename(outputFilename);
        std::tie(outputName, outputExtension) = bin2h::getNameAndExtension(outputFilePath);
        std::string predefinedSymbol = bin2h::convertSymbol(outputName, bin2h::CaseNotation::upperSnakeCase) + "_H";

        out << fmt::format("#ifndef {}", predefinedSymbol) << std::endl;
        out << fmt::format("#define {}", predefinedSymbol) << std::endl;
        out << std::endl;

        for (auto&& inputFilename: inputFilenames) {
            // get input binary file
            spdlog::info("open input file: {}", inputFilename);
            std::ifstream in(inputFilename, std::ifstream::binary);
            if (!in.is_open()) {
                throw std::ios_base::failure(fmt::format("failed to open file \"{}\"", inputFilename));
            }
            in.seekg(0, in.end);
            auto filesize = in.tellg();
            std::vector<uint8_t> data;
            data.resize((size_t)filesize);
            memset(data.data(), 255, data.size());
            in.seekg(0, in.beg);
            spdlog::info("read data of file {}: {}", inputFilename, data.size());
            in.read(reinterpret_cast<char*>(data.data()), filesize);
            if (in)
                spdlog::info("successfully read {} bytes", filesize);
            else
                spdlog::info("something wrong at {}", in.tellg());
            
            in.close();

            std::string inputDirectoryPath, inputFilePath, inputName, inputExtension;
            std::tie(inputDirectoryPath, inputFilePath) = bin2h::getDirectoryAndFilename(inputFilename);
            std::tie(inputName, inputExtension) = bin2h::getNameAndExtension(inputFilePath);

            auto arrayName = bin2h::convertSymbol(inputName, bin2h::CaseNotation::upperSnakeCase);
            auto sizeName = arrayName + "_SIZE";
            std::stringstream commentString;
            size_t alignedSize = (static_cast<size_t>(filesize) + align - 1) / align * align;
            out << fmt::format("static const unsigned long {} = {};", sizeName, alignedSize) << std::endl;
            out << fmt::format("static const unsigned char {}[{}] = ", arrayName, sizeName) << '{' << std::endl;

            for (size_t i = 0; i < alignedSize; ++i) {
                if (i % bytePerLine == 0) {
                    out << "    ";
                    commentString.str(std::string());
                    commentString << "// ";
                }
                uint8_t elem = (i < data.size() ? data[i] : 0);
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

    return 0;
}