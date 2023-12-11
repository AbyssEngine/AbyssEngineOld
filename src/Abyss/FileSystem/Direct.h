#pragma once

#include <filesystem>
#include <string_view>
#include <string>
#include <unordered_map>
#include <vector>

#include "InputStream.h"
#include "Provider.h"

namespace Abyss::FileSystem {

class Direct final : public Provider {
    std::filesystem::path _basePath;
    // Makes the filenames case insensitive regardless of OS
    std::unordered_map<std::string, std::string> _files;

  public:
    explicit Direct(const std::filesystem::path &path);
    bool has(std::string_view fileName) override;
    InputStream load(std::string_view fileName) override;
};

} // namespace Abyss::FileSystem
