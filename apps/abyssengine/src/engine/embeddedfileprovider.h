#ifndef ABYSS_EMBEDDEDFILEPROVIDER_H
#define ABYSS_EMBEDDEDFILEPROVIDER_H

#include "provider.h"
#include <libabyss/streams/inputstream.h>
#include <map>
#include <span>
#include <vector>

namespace AbyssEngine {

class EmbeddedFileProvider : public Provider {
  public:
    bool Exists(const std::filesystem::path &path) override;
    LibAbyss::InputStream Load(const std::filesystem::path &path) override;
    void AddFile(const std::filesystem::path &path, const std::span<uint8_t> data);

  private:
    std::map<std::string, const std::span<uint8_t>> _files;
};

} // namespace AbyssEngine

#endif // ABYSS_EMBEDDEDFILEPROVIDER_H
