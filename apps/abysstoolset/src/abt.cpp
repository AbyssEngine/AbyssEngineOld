#include "config.h"
#include "libabyss/formats/d2/dc6.h"
#include "libabyss/streams/mpqstream.h"
#include <argh.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <png.h>
#include <spdlog/spdlog.h>
#include <vector>

namespace {
void ConvertDC6ToPNG(std::string path) {
    auto basePath = std::filesystem::absolute(path).lexically_normal();
    SPDLOG_INFO("Converting ALL DC6 files in {0} to PNG", basePath.string());

    std::vector<std::filesystem::path> dc6Files;
    for (const auto &entry : std::filesystem::recursive_directory_iterator(basePath)) {
        if (entry.path().extension() == ".dc6") {
            dc6Files.push_back(entry.path());
        }
    }

    for (const auto &dc6File : dc6Files) {
        SPDLOG_INFO("Converting {0}", dc6File.string());

        {
            auto pngPath = dc6File;
            pngPath.replace_extension("");

            auto fileBuff = std::make_unique<std::filebuf>();
            fileBuff->open(dc6File.string(), std::ios::in | std::ios::binary);

            LibAbyss::InputStream inputStream(std::move(fileBuff));
            auto dc6 = LibAbyss::DC6(inputStream);

            int directionIndex = 0;
            for (const auto &direction : dc6.Directions) {
                const auto pathDir = pngPath.string() + "_" + (std::to_string(directionIndex));

                int frameIndex = 0;
                for (const auto &frame : direction.Frames) {
                    auto pngFile = pathDir + "_" + (std::to_string(frameIndex) + ".png");
                    SPDLOG_INFO("   -> Writing {0}", pngFile);

                    auto fp = fopen(pngFile.c_str(), "wb");
                    auto png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
                    auto pngInfo = png_create_info_struct(png);
                    png_init_io(png, fp);

                    png_set_IHDR(png, pngInfo, frame.Width, frame.Height, 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                                 PNG_FILTER_TYPE_DEFAULT);
                    png_write_info(png, pngInfo);
                    auto row = new png_byte[frame.Width];

                    uint8_t maxValue = 0;
                    for (const auto &idx : frame.IndexData) {
                        if (idx > maxValue) {
                            maxValue = idx;
                        }
                    }

                    float numberMod = (float)maxValue / 255.0f;

                    for (int y = 0; y < frame.Height; y++) {
                        for (int x = 0; x < frame.Width; x++) {
                            row[x] = (uint8_t)((float)frame.IndexData.at(x + (y * frame.Width)) * numberMod);
                        }
                        png_write_row(png, row);
                    }

                    png_write_end(png, pngInfo);

                    delete[] row;
                    png_free_data(png, pngInfo, PNG_FREE_ALL, -1);
                    png_destroy_write_struct(&png, nullptr);
                    fclose(fp);

                    frameIndex++;
                }
                directionIndex++;
            }
        }
    }

    return;
}

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
} // namespace

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

    {
        std::string dc6ToPng;
        if (commandLine({"--dc6-to-png"}) >> dc6ToPng) {
            ConvertDC6ToPNG(dc6ToPng);
            return EXIT_SUCCESS;
        }
    }

    return EXIT_SUCCESS;
}
