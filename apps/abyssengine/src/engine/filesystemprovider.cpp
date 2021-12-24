#include "filesystemprovider.h"

#include <fstream>
#include <utility>

namespace AbyssEngine {

FileSystemProvider::FileSystemProvider(std::filesystem::path path) : _basePath(std::move(path)) {}

bool FileSystemProvider::Exists(const std::filesystem::path &path) { return exists(std::filesystem::absolute(_basePath / path)); }

LibAbyss::InputStream FileSystemProvider::Load(const std::filesystem::path &path) {
    auto fb = std::make_unique<std::filebuf>();
    fb->open(_basePath / path, std::ios_base::binary | std::ios_base::in);

    return LibAbyss::InputStream(std::move(fb));
}

} // namespace AbyssEngine
