#include "config.h"
#include <argh.h>
#include <cstdlib>
#include <filesystem>
#include "libabyss/mpqstream.h"
#include <spdlog/spdlog.h>

void ExtractMPQ(std::string_view mpqFile, const std::filesystem::path &outputPath) {
    SPDLOG_INFO("Extracting {0} to {1}", mpqFile, outputPath.string());

    if (!exists(outputPath))
        std::filesystem::create_directories(outputPath);

    LibAbyss::MPQ mpq(mpqFile);

    for (const auto &line : mpq.FileList()) {

        if (!mpq.HasFile(line))
            continue;

        std::string destFile = line;
        std::replace(destFile.begin(), destFile.end(), '\\', '/');
        auto filePath = (outputPath / destFile).make_preferred();

        try {
            std::ofstream writeFile(filePath.string(), std::ios::out | std::ios::binary);
            auto readFile = mpq.Load(line);
            SPDLOG_INFO("Extracting {0}", filePath.string());
            if (!exists(filePath.remove_filename()))
                std::filesystem::create_directories(filePath.remove_filename());

            char buff[4096];
            while (true) {
                readFile.read(buff, 4096);
                auto toWrite = readFile.gcount();
                if (toWrite <= 0)
                    break;
                writeFile.write(buff, toWrite);
            }

        } catch (std::exception &ex) {
            SPDLOG_ERROR("Error reading {0}", filePath.string());
        }
    }
}

void ListMPQ(std::string_view mpqFile) {
    std::filesystem::path mpqPath(mpqFile);
    mpqPath = std::filesystem::absolute(mpqPath);

    SPDLOG_INFO("Listing contents of {0}", mpqPath.string());

    LibAbyss::MPQ mpq(mpqPath);

    if (!mpq.HasFile("(listfile)")) {
        SPDLOG_ERROR("MPQ does not contain a listfile.");
    }

    for (const auto &line : mpq.FileList()) {
        SPDLOG_INFO(line);
    }
}

void ListMPQContents(std::string_view mpqFile) { SPDLOG_INFO("Listing contents of {0}:", mpqFile); }

int main(int, char *argv[]) {
    SPDLOG_INFO(ABYSS_VERSION_STRING);

    argh::parser commandLine(argv);

    if (commandLine[{"-v", "--verbose"}]) {
        spdlog::set_level(spdlog::level::trace);
        SPDLOG_INFO("Verbose logging enabled");
    } else {
        spdlog::set_level(spdlog::level::info);
    }

    {
        std::string extractPath;
        if (commandLine({"-e", "--extract"}) >> extractPath) {
            std::filesystem::path outPath(commandLine({"-o", "--outpath"}).str());
            ExtractMPQ(extractPath, absolute(outPath));

            return EXIT_SUCCESS;
        }
    }

    {
        std::string path;
        if (commandLine({"-l", "--list"}) >> path) {
            ListMPQ(path);

            return EXIT_SUCCESS;
        }
    }

    {
        std::string extractPath;
        if (commandLine({"-l", "--list"}) >> extractPath) {
            ListMPQContents(extractPath);

            return EXIT_SUCCESS;
        }
    }

    return EXIT_SUCCESS;
}
