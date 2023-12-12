#pragma once

#include <filesystem>
#include <string_view>
#include <string>
#include <vector>

#include "InputStream.h"
#include "Provider.h"

namespace Abyss::FileSystem {

class CASC final : public Provider {
    void* _storage;

  public:
    explicit CASC(const std::filesystem::path &cascPath);
    ~CASC() override;
    bool has(std::string_view fileName) override;
    InputStream load(std::string_view fileName) override;
};

} // namespace Abyss::FileSystem
