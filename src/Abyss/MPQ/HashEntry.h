#pragma once

#include <cstdint>

namespace Abyss::MPQ {
class HashEntry {
    uint32_t hashA;
    uint32_t hashB;
    uint16_t locale;
    uint16_t platform;
    uint32_t blockIndex;

  public:
    HashEntry(uint32_t hashA, uint32_t hashB, uint16_t locale, uint16_t platform, uint32_t blockIndex);

    [[nodiscard]] auto Name64() const -> uint64_t;
    [[nodiscard]] auto getHashA() const -> uint32_t;
    [[nodiscard]] auto getHashB() const -> uint32_t;
    [[nodiscard]] auto getLocale() const -> uint16_t;
    [[nodiscard]] auto getPlatform() const -> uint16_t;
    [[nodiscard]] auto getBlockIndex() const -> uint32_t;
};
} // namespace Abyss::MPQ
