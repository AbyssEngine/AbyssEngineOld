#include "cascprovider.h"

AbyssEngine::CASCProvider::CASCProvider(const std::filesystem::path &path) : _casc(path) {

}

bool AbyssEngine::CASCProvider::Exists(const std::filesystem::path &path) {
    return _casc.HasFile(path.string());
}

LibAbyss::InputStream AbyssEngine::CASCProvider::Load(const std::filesystem::path &path) {
    return _casc.Load(path.string());
}
