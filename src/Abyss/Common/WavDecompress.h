#pragma once

#include <array>
#include <cstdint>
#include <span>
#include <vector>

namespace Abyss::Common {

auto WavDecompress(const std::span<std::byte> &input, uint8_t channelCount) -> std::vector<std::byte>;

} // namespace Abyss::Common
