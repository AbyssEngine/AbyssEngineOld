module;

#include <cxxopts.hpp>
#include <filesystem>
#include <string>

import Abyss.Common.Configuration;
import Abyss.Common.Logging;

export module Abyss.Common.CommandLineOpts;

namespace Abyss::Common::CommandLineOpts {

export auto process(int argc, char **argv, bool &quitOnRun, Configuration &config) -> void {
    cxxopts::Options options("Abyss", "Abyss Engine");
    options.add_options()                                                //
        ("d,mpqdir", "Path to MPQ files", cxxopts::value<std::string>()) //
        ("o,loadorder", "Comma separated list of MPQ files to load", cxxopts::value<std::string>())("h,help", "Print usage");

    auto result = options.parse(argc, argv);

    if (result.count("help") != 0U) {
        Log::info("{}", options.help());
        quitOnRun = true;
        return;
    }

    if (result.count("mpqdir") != 0U) {
        const auto mpqDir = result["mpqdir"].as<std::string>();

        if (!std::filesystem::exists(mpqDir)) {
            Log::error("MPQ directory does not exist: {}", mpqDir);
            exit(1);
        }

        if (!std::filesystem::is_directory(mpqDir)) {
            Log::error("MPQ directory is not a directory: {}", mpqDir);
            exit(1);
        }

        config.setMPQDir(mpqDir);
        Log::info("Using MPQ directory: {}", mpqDir);
    } else {
#ifdef _WIN32
        if (std::filesystem::is_directory("Z:/Documents/MPQs")) {
            config.setMPQDir("Z:/Documents/MPQs");
        }
#endif // _WIN32
    }

    if (result.count("loadorder") == 0U) {
        Log::info("Using default MPQ load order");
    } else {
        std::string const loadOrderString = result["loadorder"].as<std::string>();
        std::stringstream stringStream(loadOrderString);
        std::string item;

        std::vector<std::filesystem::path> loadOrder;

        while (std::getline(stringStream, item, ',')) {
            std::filesystem::path const mpqFile = config.getMPQDir() / item;

            if (!exists(mpqFile)) {
                Log::error("MPQ file does not exist: {}", mpqFile.string());
                exit(1);
            }

            loadOrder.push_back(mpqFile);
        }

        config.setLoadOrder(std::move(loadOrder));
    }
}

} // namespace Abyss::Common::CommandLineOpts
