#pragma once

#include "InputStream.h"
#include "Provider.h"

#include <absl/container/flat_hash_map.h>
#include <string>
#include <utility>
#include <vector>

namespace Abyss::FileSystem {

class FileLoader {
  public:
    virtual ~FileLoader() = default;

    [[nodiscard]] std::string loadString(std::string_view path);
    [[nodiscard]] std::vector<std::string> loadStringList(std::string_view path);
    [[nodiscard]] std::vector<std::byte> loadBytes(std::string_view path);
    [[nodiscard]] virtual InputStream loadFile(std::string_view path) = 0;
    [[nodiscard]] virtual bool fileExists(std::string_view path) = 0;
};

class MultiFileLoader final : public FileLoader {
  public:
    ~MultiFileLoader() override = default;
    [[nodiscard]] InputStream loadFile(std::string_view path) override;
    [[nodiscard]] bool fileExists(std::string_view path) override;
    void addProvider(std::unique_ptr<Provider> provider);

  private:
    std::vector<std::unique_ptr<Provider>> _providers;
    absl::flat_hash_map<std::string, int /* index in _providers */> _cacheWhere;
};

} // namespace Abyss::FileSystem
