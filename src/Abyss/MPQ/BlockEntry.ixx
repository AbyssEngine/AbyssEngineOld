module;

#include <cstdint>
#include <string_view>

import Abyss.MPQ.Crypto;

export module Abyss.MPQ.BlockEntry;

namespace Abyss::MPQ {

export class MPQBlockEntry {
    enum class Flags : uint32_t {
        FILE_IMPLODE = 0x00000100,
        FILE_COMPRESS = 0x00000200,
        FILE_ENCRYPTED = 0x00010000,
        FILE_FIX_KEY = 0x00020000,
        FILE_PATCH_FILE = 0x00100000,
        FILE_SINGLE_UNIT = 0x01000000,
        FILE_DELETE_MARKER = 0x02000000,
        FILE_SECTOR_CRC = 0x04000000,
        FILE_EXISTS = 0x80000000
    };
    uint32_t filePosition;
    uint32_t compressedSize;
    uint32_t uncompressedSize;
    Flags flags;
    uint32_t encryptionSeed{};

    [[nodiscard]] auto hasFlag(Flags flag) const -> bool { return (static_cast<uint32_t>(flags) & static_cast<uint32_t>(flag)) != 0; }

  public:
    MPQBlockEntry(const uint32_t filePosition, const uint32_t compressedSize, const uint32_t uncompressedSize,
                  uint32_t flags) // NOLINT(*-easily-swappable-parameters)
        : filePosition(filePosition), compressedSize(compressedSize), uncompressedSize(uncompressedSize), flags(static_cast<Flags>(flags)) {}

    [[nodiscard]] auto getFilePosition() const -> uint32_t { return filePosition; }

    [[nodiscard]] auto getCompressedSize() const -> uint32_t { return compressedSize; }

    [[nodiscard]] auto getUncompressedSize() const -> uint32_t { return uncompressedSize; }

    [[nodiscard]] auto getFlags() const -> Flags { return flags; }

    [[nodiscard]] auto getEncryptionSeed() const -> uint32_t { return encryptionSeed; }

    [[nodiscard]] auto isEncrypted() const -> bool { return hasFlag(Flags::FILE_ENCRYPTED); }

    [[nodiscard]] auto isCompressed() const -> bool { return hasFlag(Flags::FILE_COMPRESS); }

    [[nodiscard]] auto isSingleUnit() const -> bool { return hasFlag(Flags::FILE_SINGLE_UNIT); }

    [[nodiscard]] auto isImploded() const -> bool { return hasFlag(Flags::FILE_IMPLODE); }

    [[nodiscard]] auto isPatched() const -> bool { return hasFlag(Flags::FILE_PATCH_FILE); }

    [[nodiscard]] auto isDeleted() const -> bool { return hasFlag(Flags::FILE_DELETE_MARKER); }

    [[nodiscard]] auto hasSectorCRC() const -> bool { return hasFlag(Flags::FILE_SECTOR_CRC); }

    [[nodiscard]] auto exists() const -> bool { return hasFlag(Flags::FILE_EXISTS); }

    [[nodiscard]] auto hasFixKey() const -> bool { return hasFlag(Flags::FILE_FIX_KEY); }

    auto calculateEncryptionSeed(const std::string_view name) -> void {
        const auto extractedFileName = name.substr(name.find_last_of('/') + 1);
        const auto seed = Crypto::getInstance().hashString(extractedFileName, 3);
        encryptionSeed = (seed + filePosition) ^ uncompressedSize;
    }
};
} // namespace Abyss::MPQ
