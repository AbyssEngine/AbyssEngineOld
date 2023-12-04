#pragma once

#include <array>
#include <filesystem>
#include <vector>

namespace Abyss::Common {

class Configuration {
    static constexpr std::array<std::string_view, 11> MPQ_LOAD_ORDER = {
        "patch_d2.mpq", "d2exp.mpq",   "d2xmusic.mpq", "d2xtalk.mpq", "d2xvideo.mpq", "d2data.mpq",
        "d2char.mpq",   "d2music.mpq", "d2sfx.mpq",    "d2video.mpq", "d2speech.mpq",
    };
    std::filesystem::path _mpqDir;
    std::vector<std::filesystem::path> _loadOrder;

  public:
    auto getLoadOrder() -> const std::vector<std::filesystem::path> &;
    void setLoadOrder(std::vector<std::filesystem::path> newLoadOrder);
    auto getMPQDir() -> const std::filesystem::path &;
    void setMPQDir(std::filesystem::path newDir);
};

} // namespace Abyss::Common
