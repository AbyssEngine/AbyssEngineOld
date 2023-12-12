#pragma once

#include <absl/container/flat_hash_map.h>
#include <string>
#include <string_view>

namespace OD2::Common {

class SoundManager {
    SoundManager();
    absl::flat_hash_map<std::string /* name */, std::string /* filename */> _cache;
    absl::flat_hash_map<std::string /* name */, int> _index;

  public:
    static SoundManager &getInstance() {
        static SoundManager instance;
        return instance;
    }

    const std::string &getSound(std::string_view name);
};

} // namespace OD2::Common
