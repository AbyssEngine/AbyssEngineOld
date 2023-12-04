#pragma once

#include <Abyss/MPQ/Crypto.h>

namespace Abyss::MPQ {
class MPQBlockEntry {
  private:
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

    [[nodiscard]] auto hasFlag(Flags flag) const -> bool;

  public:
    MPQBlockEntry(uint32_t filePosition, uint32_t compressedSize, uint32_t uncompressedSize, uint32_t flags);

    [[nodiscard]] auto getFilePosition() const -> uint32_t;

    [[nodiscard]] auto getCompressedSize() const -> uint32_t;

    [[nodiscard]] auto getUncompressedSize() const -> uint32_t;

    [[nodiscard]] auto getFlags() const -> Flags;

    [[nodiscard]] auto getEncryptionSeed() const -> uint32_t;

    [[nodiscard]] auto isEncrypted() const -> bool;

    [[nodiscard]] auto isCompressed() const -> bool;

    [[nodiscard]] auto isSingleUnit() const -> bool;

    [[nodiscard]] auto isImploded() const -> bool;

    [[nodiscard]] auto isPatched() const -> bool;

    [[nodiscard]] auto isDeleted() const -> bool;

    [[nodiscard]] auto hasSectorCRC() const -> bool;

    [[nodiscard]] auto exists() const -> bool;

    [[nodiscard]] auto hasFixKey() const -> bool;

    void calculateEncryptionSeed(std::string_view name);
};
} // namespace Abyss::MPQ
