module;

#include <array>
#include <cstdint>

export module Abyss.MPQ.Header;

namespace Abyss::MPQ {

export struct Header {
    std::array<char, 4> magic;
    uint32_t headerSize;
    uint32_t archiveSize;
    uint16_t formatVersion;
    uint16_t blockSize;
    uint32_t hashTableOffset;
    uint32_t blockTableOffset;
    uint32_t hashTableEntries;
    uint32_t blockTableEntries;
};
} // namespace Abyss::MPQ
