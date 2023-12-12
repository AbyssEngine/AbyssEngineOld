#include "FileLoader.h"

namespace Abyss::FileSystem {

std::string FileLoader::loadString(std::string_view path) {
    auto stream = loadFile(path);
    std::string result;
    result.resize(stream.size());
    stream.read(result.data(), stream.size());
    return result;
}

std::vector<std::string> FileLoader::loadStringList(std::string_view path) {
    auto stream = loadFile(path);
    std::vector<std::string> result;
    std::string line;
    while (std::getline(stream, line)) {
        if (line.empty()) {
            continue;
        }
        result.push_back(std::move(line));
    }
    return result;
}

std::vector<std::byte> FileLoader::loadBytes(std::string_view path) {
    auto stream = loadFile(path);
    std::vector<std::byte> result;
    result.resize(stream.size());
    stream.read(std::bit_cast<char *>(result.data()), stream.size());
    return result;
}

InputStream MultiFileLoader::loadFile(std::string_view path) {
    auto it = _cacheWhere.find(path);
    if (it != _cacheWhere.end()) {
        return _providers[it->second]->load(path);
    }

    int i = 0;
    for (const auto &provider : _providers) {
        if (provider->has(path)) {
          _cacheWhere[path] = i;
            return provider->load(path);
        }
        ++i;
    }

    throw std::runtime_error(absl::StrCat("File not found: ", path));
}

bool MultiFileLoader::fileExists(std::string_view path) {
    auto it = _cacheWhere.find(path);
    if (it != _cacheWhere.end()) {
        return it->second != -1;
    }

    int i = 0;
    for (const auto &provider : _providers) {
        if (provider->has(path)) {
            _cacheWhere[path] = i;
            return true;
        }
        ++i;
    }

    _cacheWhere[path] = -1;
    return false;
}

void MultiFileLoader::addProvider(std::unique_ptr<Provider> provider) {
    auto lock = std::lock_guard(_mutex);
    _providers.push_back(std::move(provider));
}

} // namespace Abyss::FileSystem
