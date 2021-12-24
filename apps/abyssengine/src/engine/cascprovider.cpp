#include "cascprovider.h"

namespace AbyssEngine {

CASCProvider::CASCProvider(const std::filesystem::path &path) : _casc(path) {}

bool CASCProvider::Exists(const std::filesystem::path &path) { return _casc.HasFile(path.string()); }

LibAbyss::InputStream CASCProvider::Load(const std::filesystem::path &path) { return _casc.Load(path.string()); }

} // namespace AbyssEngine
