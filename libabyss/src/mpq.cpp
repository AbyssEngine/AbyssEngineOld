#include "../include/libabyss/mpq.h"
#include "../include/libabyss/mpqstream.h"
#include <absl/strings/str_format.h>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>

#define STORMLIB_NO_AUTO_LINK 1
#include <StormLib.h>

LibAbyss::MPQ::MPQ(const std::filesystem::path &mpqPath) : _mpqPath(std::filesystem::absolute(mpqPath).string()), _stormMpq() {
    if (!SFileOpenArchive(_mpqPath.c_str(), 0, STREAM_PROVIDER_FLAT | BASE_PROVIDER_FILE | STREAM_FLAG_READ_ONLY, &_stormMpq)) {
        throw std::runtime_error(absl::StrFormat("Error occurred while opening MPQ %s", mpqPath.string()));
    }
}

LibAbyss::MPQ::~MPQ() { SFileCloseArchive(_stormMpq); }

LibAbyss::InputStream LibAbyss::MPQ::Load(std::string_view fileName) {
    return LibAbyss::InputStream(std::make_unique<LibAbyss::MPQStream>(_stormMpq, FixPath(fileName)));
}

bool LibAbyss::MPQ::HasFile(std::string_view fileName) { return SFileHasFile(_stormMpq, FixPath(fileName).c_str()); }

std::vector<std::string> LibAbyss::MPQ::FileList() {
    std::vector<std::string> result;

    if (!HasFile("(listfile)")) {
        SPDLOG_ERROR("MPQ does not contain a listfile.");
    }

    auto stream = Load("(listfile)");

    std::string line;
    while (true) {
        std::getline(stream, line);
        if (stream.eof()) {
            break;
        }

        // Remove the \r character from the line
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

        result.push_back(line);
    }

    return result;
}

std::string LibAbyss::MPQ::FixPath(std::string_view str) {
    std::string result(str);
    std::replace(result.begin(), result.end(), '/', '\\');
    if (result.starts_with('\\')) {
        return result.substr(1);
    }

    return result;
}
