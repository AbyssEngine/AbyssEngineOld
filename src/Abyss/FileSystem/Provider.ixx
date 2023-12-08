module;

#include <filesystem>

export module Abyss.FileSystem.Provider;

import Abyss.FileSystem.InputStream;

namespace Abyss::FileSystem {

export class Provider {
  public:
    virtual ~Provider() = default;
    virtual bool has(std::string_view path) = 0;
    virtual InputStream load(std::string_view path) = 0;
};

} // namespace Abyss::FileSystem
