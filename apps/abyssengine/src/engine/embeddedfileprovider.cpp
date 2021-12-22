#include "embeddedfileprovider.h"
#include <absl/strings/ascii.h>
#include <istream>
#include <sstream>
#include <ios>

class vectorwrapbuf : public std::basic_streambuf<uint8_t> {
  public:
    vectorwrapbuf(std::vector<uint8_t> &vec) { setg(vec.data(), vec.data(), vec.data() + vec.size()); }
};

bool AbyssEngine::EmbeddedFileProvider::Exists(const std::filesystem::path &path) { return _files.contains(absl::AsciiStrToLower(path.string())); }

LibAbyss::InputStream AbyssEngine::EmbeddedFileProvider::Load(const std::filesystem::path &path) {
    auto file = _files[absl::AsciiStrToLower(path.string())];
    std::string str(file.begin(), file.end());
    auto sb = std::make_unique<std::stringbuf>();
    sb->str(str);
    return LibAbyss::InputStream(std::move(sb));
}

void AbyssEngine::EmbeddedFileProvider::AddFile(const std::filesystem::path &path, std::span<uint8_t> data) {
    _files.emplace(absl::AsciiStrToLower(path.string()), data);
}
