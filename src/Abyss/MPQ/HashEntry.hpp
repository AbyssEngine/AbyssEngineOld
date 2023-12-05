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
    HashEntry(const uint32_t hashA, const uint32_t hashB, const uint16_t locale, const uint16_t platform,
              const uint32_t blockIndex) // NOLINT(*-easily-swappable-parameters)
        : hashA(hashA), hashB(hashB), locale(locale), platform(platform), blockIndex(blockIndex) {}

    [[nodiscard]] auto Name64() const -> uint64_t { return (static_cast<uint64_t>(hashB) << 32) | hashA; }

    [[nodiscard]] auto getHashA() const -> uint32_t { return hashA; }

    [[nodiscard]] auto getHashB() const -> uint32_t { return hashB; }

    [[nodiscard]] auto getLocale() const -> uint16_t { return locale; }

    [[nodiscard]] auto getPlatform() const -> uint16_t { return platform; }

    [[nodiscard]] auto getBlockIndex() const -> uint32_t { return blockIndex; }
};

} // namespace Abyss::MPQ
