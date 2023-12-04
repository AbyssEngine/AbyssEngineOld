#include "Configuration.h"

namespace Abyss::Common {

auto Configuration::getLoadOrder() -> const std::vector<std::filesystem::path> & { return _loadOrder; }

void Configuration::setLoadOrder(std::vector<std::filesystem::path> newLoadOrder) { this->_loadOrder = std::move(newLoadOrder); }

auto Configuration::getMPQDir() -> const std::filesystem::path & { return _mpqDir; }

void Configuration::setMPQDir(std::filesystem::path newDir) {
    this->_mpqDir = std::move(newDir);

    if (!_mpqDir.empty()) {
        for (const auto &mpqFile : MPQ_LOAD_ORDER) {
            if (std::filesystem::path const mpqPath = _mpqDir / mpqFile; std::filesystem::exists(mpqPath)) {
                _loadOrder.push_back(mpqPath);
            }
        }
    }
}

} // namespace Abyss::Common
