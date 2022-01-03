#ifndef ABYSS_TTFMANAGER_H
#define ABYSS_TTFMANAGER_H

#include <absl/container/flat_hash_map.h>
#include <filesystem>
#include <fontconfig/fontconfig.h>
#include <memory>
#include <string>

namespace AbyssEngine {
class TtfManager {
  public:
    TtfManager();
    ~TtfManager();
    void AddFont(const std::filesystem::path &path);

  private:
    FcConfig *_fcConfig;
    absl::flat_hash_map<std::string, std::filesystem::path> _knownFonts;
};
} // namespace AbyssEngine

#endif // ABYSS_TTFMANAGER_H
