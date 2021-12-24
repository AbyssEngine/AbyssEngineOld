#include "loader.h"
#include <absl/strings/str_cat.h>

namespace AbyssEngine {

Loader::Loader() : _providers() {}

void Loader::AddProvider(std::unique_ptr<Provider> provider) { _providers.push_back(std::move(provider)); }

bool Loader::FileExists(const std::filesystem::path &path) {
    for (const auto &provider : _providers)
        if (provider->Exists(path))
            return true;

    return false;
}

LibAbyss::InputStream Loader::Load(const std::filesystem::path &path) {
    for (const auto &provider : _providers)
        if (provider->Exists(path))
            return provider->Load(path);

    throw std::runtime_error(absl::StrCat("File not found: ", path.string()));
}

} // namespace AbyssEngine
