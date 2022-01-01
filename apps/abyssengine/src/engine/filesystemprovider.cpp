#include "filesystemprovider.h"

#include <absl/strings/ascii.h>
#include <absl/strings/str_replace.h>
#include <absl/strings/strip.h>
#include <fstream>
#include <utility>

namespace AbyssEngine {

// Removes / if prepended, and turns lower case
static std::string NormalizePath(const std::filesystem::path &path) {
    std::string p = path.string();
    absl::AsciiStrToLower(&p);
    return std::string(absl::StripPrefix(p, "/"));
}

FileSystemProvider::FileSystemProvider(std::filesystem::path path) : _basePath(std::move(path)) {
    for (const auto &entry : std::filesystem::recursive_directory_iterator(_basePath)) {
        if (!entry.is_regular_file())
            continue;
        std::string p = entry.path().lexically_relative(_basePath).string();
        std::string lower = absl::AsciiStrToLower(absl::StrReplaceAll(p, {{"\\", "/"}}));
        _files[std::move(lower)] = std::move(p);
    }
}

bool FileSystemProvider::Exists(const std::filesystem::path &path) { return _files.contains(NormalizePath(path)); }

LibAbyss::InputStream FileSystemProvider::Load(const std::filesystem::path &path) {
    auto fb = std::make_unique<std::filebuf>();
    fb->open(_basePath / _files.at(NormalizePath(path)), std::ios_base::binary | std::ios_base::in);

    return LibAbyss::InputStream(std::move(fb));
}

} // namespace AbyssEngine
