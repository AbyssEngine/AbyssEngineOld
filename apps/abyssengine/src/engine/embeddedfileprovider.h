#ifndef ABYSS_EMBEDDEDFILEPROVIDER_H
#define ABYSS_EMBEDDEDFILEPROVIDER_H

#include "provider.h"
#include <libabyss/streams/inputstream.h>

namespace AbyssEngine {

class EmbeddedFileProvider : public Provider {
  public:
    bool Exists(const std::filesystem::path &path) override;
    LibAbyss::InputStream Load(const std::filesystem::path &path) override;
};

} // namespace AbyssEngine

#endif // ABYSS_EMBEDDEDFILEPROVIDER_H
