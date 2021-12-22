#include "embeddedfileprovider.h"
#include <absl/strings/ascii.h>
#include <cmrc/cmrc.hpp>
#include <ios>
#include <istream>
#include <sstream>

CMRC_DECLARE(AbyssEngine);

namespace AbyssEngine {

bool EmbeddedFileProvider::Exists(const std::filesystem::path &path) {
    auto fs = cmrc::AbyssEngine::get_filesystem();
    return fs.exists(path.string());
}

LibAbyss::InputStream EmbeddedFileProvider::Load(const std::filesystem::path &path) {
    auto fs = cmrc::AbyssEngine::get_filesystem();
    auto file = fs.open(path.string());
    std::string data(file.begin(), file.size());

    // TODO use std::spanbuf in C++23
    auto sb = std::make_unique<std::stringbuf>();
    sb->str(std::move(data));
    return LibAbyss::InputStream(std::move(sb));
}

} // namespace AbyssEngine
