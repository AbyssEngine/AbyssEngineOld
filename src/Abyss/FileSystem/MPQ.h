#pragma once

#include <filesystem>
#include <string_view>
#include <string>
#include <vector>

#include "InputStream.h"
#include "Provider.h"

namespace Abyss::FileSystem {

class MPQ final : public Provider {
    void* _stormMpq;

  public:
    /// Proxy constructor that creates an MPQ based on the specified filename.
    /// \param mpqPath Path to the MPQ file to load.
    explicit MPQ(const std::filesystem::path &mpqPath);
    ~MPQ() override;
    bool has(std::string_view fileName) override;
    InputStream load(std::string_view fileName) override;
    std::vector<std::string> fileList();
};

} // namespace Abyss::FileSystem
