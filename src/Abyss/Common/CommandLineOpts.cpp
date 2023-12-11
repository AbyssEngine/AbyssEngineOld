#include "CommandLineOpts.h"

#include "Logging.h"
#include <cxxopts.hpp>

namespace Abyss::Common::CommandLineOpts {

void process(int argc, char **argv, bool &quitOnRun, Configuration &config) {
    cxxopts::Options options("Abyss", "Abyss Engine");
    options.add_options()                                                //
        ("d,mpqdir", "Path to MPQ files", cxxopts::value<std::string>()) //
        ("cascdir", "Path to CASC dir", cxxopts::value<std::string>()) //
        ("direct", "Path to dir", cxxopts::value<std::string>()) //
        ("o,loadorder", "Comma separated list of MPQ files to load", cxxopts::value<std::string>())("h,help", "Print usage");

    auto result = options.parse(argc, argv);

    if (result.count("help") != 0U) {
        Log::info("{}", options.help());
        quitOnRun = true;
        return;
    }

    auto checkDir = [](const std::string& s) {
        if (!std::filesystem::exists(s)) {
            Log::error("Directory does not exist: {}", s);
            exit(1);
        }

        if (!std::filesystem::is_directory(s)) {
            Log::error("Directory is not a directory: {}", s);
            exit(1);
        }
    };

    if (result.count("direct") != 0U) {
        const auto dir = result["direct"].as<std::string>();
        checkDir(dir);
        config.setDirectDir(dir);
        Log::info("Using directory: {}", dir);
    }

    if (result.count("cascdir") != 0U) {
        const auto cascDir = result["cascdir"].as<std::string>();
        checkDir(cascDir);
        config.setCASCDir(cascDir);
        Log::info("Using CASC directory: {}", cascDir);
    }

    if (result.count("mpqdir") != 0U) {
        const auto mpqDir = result["mpqdir"].as<std::string>();
        checkDir(mpqDir);
        config.setMPQDir(mpqDir);
        Log::info("Using MPQ directory: {}", mpqDir);
    } else {
#ifdef _WIN32
        if (std::filesystem::is_directory("C:/Program Files (x86)/Diablo II")) {
            config.setMPQDir("C:/Program Files (x86)/Diablo II");
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
