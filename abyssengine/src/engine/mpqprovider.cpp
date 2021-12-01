#include "mpqprovider.h"

AbyssEngine::MPQProvider::MPQProvider(const std::filesystem::path &path) : _mpq(path) {

}

bool AbyssEngine::MPQProvider::Exists(const std::filesystem::path &path) {
    return _mpq.HasFile(path.string());
}

LibAbyss::InputStream AbyssEngine::MPQProvider::Load(const std::filesystem::path &path) {
    return _mpq.Load(path.string());
}
