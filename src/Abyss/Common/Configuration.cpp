#include "Configuration.h"

#include <array>
#include <filesystem>
#include <vector>

namespace Abyss::Common {

static constexpr std::array<std::string_view, 11> MPQ_LOAD_ORDER = {
    "patch_d2.mpq", "d2exp.mpq",   "d2xmusic.mpq", "d2xtalk.mpq", "d2xvideo.mpq", "d2data.mpq",
    "d2char.mpq",   "d2music.mpq", "d2sfx.mpq",    "d2video.mpq", "d2speech.mpq",
};

const std::vector<std::filesystem::path> &Configuration::getLoadOrder() { return _loadOrder; }

void Configuration::setLoadOrder(std::vector<std::filesystem::path> newLoadOrder) { this->_loadOrder = std::move(newLoadOrder); }

const std::filesystem::path &Configuration::getMPQDir() { return _mpqDir; }
const std::filesystem::path &Configuration::getCASCDir() { return _cascDir; }

void Configuration::setCASCDir(std::filesystem::path newDir) {
    this->_cascDir = std::move(newDir);
}

void Configuration::setMPQDir(std::filesystem::path newDir) {
    this->_mpqDir = std::move(newDir);

    if (!_mpqDir.empty()) {
        for (const auto &mpqFile : MPQ_LOAD_ORDER) {
            if (std::filesystem::path const mpqPath = _mpqDir / mpqFile; exists(mpqPath)) {
                _loadOrder.push_back(mpqPath);
            }
        }
    }
}

} // namespace Abyss::Common
