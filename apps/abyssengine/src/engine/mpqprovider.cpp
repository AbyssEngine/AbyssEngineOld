#include "mpqprovider.h"

namespace AbyssEngine {

MPQProvider::MPQProvider(const std::filesystem::path &path) : _mpq(path) {}

bool MPQProvider::Exists(const std::filesystem::path &path) { return _mpq.HasFile(path.string()); }

LibAbyss::InputStream MPQProvider::Load(const std::filesystem::path &path) { return _mpq.Load(path.string()); }

} // namespace AbyssEngine
