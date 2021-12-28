#include "filesystemprovider.h"

#include <fstream>
#include <utility>

namespace AbyssEngine {

static std::string NormalizePath(const std::filesystem::path &path) {
    // Operator / ignores the left hand path if the right hand path starts with '/', so just remove it
    std::string p = path.string();
    if (!p.empty() && p[0] == '/') {
        return p.substr(1);
    }
    return p;
}

FileSystemProvider::FileSystemProvider(std::filesystem::path path) : _basePath(std::move(path)) {}

bool FileSystemProvider::Exists(const std::filesystem::path &path) { return exists(std::filesystem::absolute(_basePath / NormalizePath(path))); }

LibAbyss::InputStream FileSystemProvider::Load(const std::filesystem::path &path) {
    auto fb = std::make_unique<std::filebuf>();
    fb->open(_basePath / NormalizePath(path), std::ios_base::binary | std::ios_base::in);

    return LibAbyss::InputStream(std::move(fb));
}

} // namespace AbyssEngine
