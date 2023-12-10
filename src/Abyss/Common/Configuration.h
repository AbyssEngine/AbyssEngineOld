#pragma once

#include <filesystem>
#include <vector>

namespace Abyss::Common {

class Configuration {
    std::filesystem::path _mpqDir;
    std::filesystem::path _cascDir;
    std::vector<std::filesystem::path> _loadOrder;

  public:
    const std::vector<std::filesystem::path> &getLoadOrder();
    void setLoadOrder(std::vector<std::filesystem::path> newLoadOrder);
    const std::filesystem::path &getMPQDir();
    const std::filesystem::path &getCASCDir();
    void setMPQDir(std::filesystem::path newDir);
    void setCASCDir(std::filesystem::path newDir);
};

} // namespace Abyss::Common
