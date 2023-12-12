#include "Direct.h"

#include <absl/strings/ascii.h>
#include <absl/strings/strip.h>
#include <algorithm>
#include <fstream>

namespace Abyss::FileSystem {

// Removes / if prepended, and turns lower case
static std::string normalizePath(std::string_view path) {
    absl::ConsumePrefix(&path, "/");
    return absl::AsciiStrToLower(path);
}

Direct::Direct(const std::filesystem::path &path) : _basePath(path) {
    for (const auto &entry : std::filesystem::recursive_directory_iterator(_basePath)) {
        if (!entry.is_regular_file())
            continue;
        std::string p = entry.path().lexically_relative(_basePath).string();
        std::string lower = p;
        std::ranges::replace(lower, '\\', '/');
        absl::AsciiStrToLower(&lower);
        _files[std::move(lower)] = std::move(p);
    }
}

bool Direct::has(std::string_view path) { return _files.contains(normalizePath(path)); }

InputStream Direct::load(std::string_view path) {
    auto fb = std::make_unique<std::filebuf>();
    fb->open(_basePath / _files.at(normalizePath(path)), std::ios_base::binary | std::ios_base::in);

    return InputStream(std::move(fb));
}

} // namespace Abyss::FileSystem
