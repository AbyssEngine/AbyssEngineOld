#pragma once

#include "InputStream.h"
#include <filesystem>

namespace Abyss::FileSystem {

class Provider {
  public:
    virtual ~Provider() = default;
    virtual bool has(std::string_view path) = 0;
    virtual InputStream load(std::string_view path) = 0;
};

} // namespace Abyss::FileSystem
