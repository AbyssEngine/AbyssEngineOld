#pragma once

#include <array>
#include <span>
#include <vector>

namespace Abyss::Common {

class WavDecompress {
  public:
    static auto decompress(const std::span<std::byte> &input, uint8_t channelCount) -> std::vector<std::byte>;
};

} // namespace Abyss::Common
