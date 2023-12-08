module;

#include <string>
#include <utility>
#include <vector>

export module Abyss.FileSystem.FileLoader;

import Abyss.FileSystem.InputStream;
import Abyss.FileSystem.Provider;

namespace Abyss::FileSystem {

export class FileLoader {
  public:
    virtual ~FileLoader() = default;

    [[nodiscard]] auto loadString(std::string_view path) -> std::string {
        auto stream = loadFile(path);
        std::string result;
        result.resize(stream.size());
        stream.read(result.data(), stream.size());
        return result;
    }

    [[nodiscard]] auto loadStringList(std::string_view path) -> std::vector<std::string> {
        auto stream = loadFile(path);
        std::vector<std::string> result;
        std::string line;
        while (std::getline(stream, line)) {
            if (line.empty()) {
                continue;
            }
            result.push_back(line);
        }
        return result;
    }

    [[nodiscard]] auto loadBytes(std::string_view path) -> std::vector<std::byte> {
        auto stream = loadFile(path);
        std::vector<std::byte> result;
        result.resize(stream.size());
        stream.read(std::bit_cast<char *>(result.data()), stream.size());
        return result;
    }

    [[nodiscard]] virtual auto loadFile(std::string_view path) -> FileSystem::InputStream = 0;
    [[nodiscard]] virtual auto fileExists(std::string_view path) -> bool = 0;
};

export class MultiFileLoader : public FileLoader {
  public:
    ~MultiFileLoader() override {}

    [[nodiscard]] auto loadFile(std::string_view path) -> FileSystem::InputStream override {
        for (const auto &provider : _providers) {
            if (provider->has(path)) {
                return provider->load(path);
            }
        }

        throw std::runtime_error("File not found: " + std::string(path));
    }

    [[nodiscard]] auto fileExists(std::string_view path) -> bool override {
        for (const auto &provider : _providers) {
            if (provider->has(path)) {
                return true;
            }
        }

        return false;
    }

    void addProvider(std::unique_ptr<Provider> provider) { _providers.push_back(std::move(provider)); }

  private:
    std::vector<std::unique_ptr<Provider>> _providers;
};

} // namespace Abyss::FileSystem
