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
    std::vector<std::string> inputFilenames;
    std::string outputFilename;
    auto cli = (
        clipp::values("input binary files").required(true).set(inputFilenames),
        (clipp::required("-o", "--output") & clipp::value("output header file").set(outputFilename)),
        clipp::option("-h", "--help").set(help, true),
        clipp::option("-v", "--verbose").set(verboseMode, true)
    );

    auto parseResult = clipp::parse(argc, argv, cli);
    if (!parseResult || help) {
        std::cout << clipp::make_man_page(cli, argv[0]);
        return -1;
    }

    if (!verboseMode)
        spdlog::set_level(verboseMode ? spdlog::level::info : spdlog::level::err);

    for (size_t i = 0; i < inputFilenames.size(); i++) {
        spdlog::info("intput filename[{}]: {}", i, inputFilenames[i]);
    }
    spdlog::info("output filename: {}", outputFilename);

    try {
        spdlog::info("open output file: {}", outputFilename);
        std::ofstream out(outputFilename);
        out << fmt::format("#ifndef") << std::endl;
        out << fmt::format("#endif") << std::endl;

        for (auto&& inputFilename: inputFilenames) {
            // get input binary file
            spdlog::info("open input file: {}", inputFilename);
            std::ifstream in(inputFilename);
            if (!in.is_open()) {
                throw std::ios_base::failure(fmt::format("failed to open file \"{}\"", inputFilename));
            }
            in.seekg(0, in.end);
            auto filesize = in.tellg();
            std::vector<uint8_t> data;
            data.resize((size_t)filesize);
            in.seekg(0, in.beg);
            spdlog::info("read data of file {}: {}", inputFilename, filesize);
            in.read(reinterpret_cast<char*>(data.data()), filesize);

            out << fmt::format("static const") << std::endl;
        }
    }
    catch (std::exception& e) {
        spdlog::error("failed to process: {}", e.what());
        return -1;
    }

    return 0;
}