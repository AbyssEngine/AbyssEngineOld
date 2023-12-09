#pragma once

#include <filesystem>
#include <vector>

namespace Abyss::Common {

class Configuration {
    std::filesystem::path _mpqDir;
    std::vector<std::filesystem::path> _loadOrder;

  public:
    const std::vector<std::filesystem::path> &getLoadOrder();
    void setLoadOrder(std::vector<std::filesystem::path> newLoadOrder);
    const std::filesystem::path &getMPQDir();
    void setMPQDir(std::filesystem::path newDir);
};

} // namespace Abyss::Common
