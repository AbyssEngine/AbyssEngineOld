#include "loader.h"
#include <absl/strings/str_cat.h>

AbyssEngine::Loader::Loader() : _mutex(), _providers() {

}

void AbyssEngine::Loader::AddProvider(std::unique_ptr<AbyssEngine::Provider> provider) {
    std::lock_guard<std::mutex> guard(_mutex);

    _providers.push_back(std::move(provider));
}

bool AbyssEngine::Loader::FileExists(const std::filesystem::path &path) {
    std::lock_guard<std::mutex> guard(_mutex);

    for (const auto &provider: _providers)
        if (provider->Exists(path))
            return true;

    return false;
}

LibAbyss::InputStream AbyssEngine::Loader::Load(const std::filesystem::path &path) {
    std::lock_guard<std::mutex> guard(_mutex);

    for (const auto &provider: _providers)
        if (provider->Exists(path))
            return provider->Load(path);

    throw std::runtime_error(absl::StrCat("File not found: ", path.string()));
}
