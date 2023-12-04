#include "HashEntry.h"

#include <cstdint>

namespace Abyss::MPQ {

HashEntry::HashEntry(uint32_t hashA, uint32_t hashB, uint16_t locale, uint16_t platform, uint32_t blockIndex) // NOLINT(*-easily-swappable-parameters)
    : hashA(hashA), hashB(hashB), locale(locale), platform(platform), blockIndex(blockIndex) {}

auto HashEntry::Name64() const -> uint64_t { return (static_cast<uint64_t>(hashB) << 32) | hashA; }

auto HashEntry::getHashA() const -> uint32_t { return hashA; }

auto HashEntry::getHashB() const -> uint32_t { return hashB; }

auto HashEntry::getLocale() const -> uint16_t { return locale; }

auto HashEntry::getPlatform() const -> uint16_t { return platform; }

auto HashEntry::getBlockIndex() const -> uint32_t { return blockIndex; }

} // namespace Abyss::MPQ
