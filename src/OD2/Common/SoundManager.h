#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

namespace OD2::Common {

class SoundManager {
    SoundManager();
    std::unordered_map<std::string /* name */, std::string /* filename */> _cache;
    std::unordered_map<std::string /* name */, int> _index;

  public:
    static SoundManager &getInstance() {
        static SoundManager instance;
        return instance;
    }

    const std::string& getSound(std::string_view name);
};

} // namespace OD2::Common
