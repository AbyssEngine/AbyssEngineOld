#include "embeddedfileprovider.h"
#include <absl/strings/ascii.h>
#include <istream>
#include <sstream>
#include <libabyss/streams/bytearraystream.h>

class vectorwrapbuf : public std::basic_streambuf<uint8_t> {
  public:
    vectorwrapbuf(std::vector<uint8_t> &vec) { setg(vec.data(), vec.data(), vec.data() + vec.size()); }
};

bool AbyssEngine::EmbeddedFileProvider::Exists(const std::filesystem::path &path) { return _files.contains(absl::AsciiStrToLower(path.string())); }

LibAbyss::InputStream AbyssEngine::EmbeddedFileProvider::Load(const std::filesystem::path &path) {
    auto data = _files[absl::AsciiStrToLower(path.string())];
    return LibAbyss::InputStream(std::make_unique<LibAbyss::ByteArrayStream>(data));
}

void AbyssEngine::EmbeddedFileProvider::AddFile(const std::filesystem::path &path, const std::vector<uint8_t> &data) {
    _files.emplace(absl::AsciiStrToLower(path.string()), data);
}
