#include "BlockEntry.h"
#include "Crypto.h"

namespace Abyss::MPQ {

MPQBlockEntry::MPQBlockEntry(const uint32_t filePosition, const uint32_t compressedSize, const uint32_t uncompressedSize,
                             uint32_t flags) // NOLINT(*-easily-swappable-parameters)
    : filePosition(filePosition), compressedSize(compressedSize), uncompressedSize(uncompressedSize), flags(static_cast<Flags>(flags)) {}

auto MPQBlockEntry::hasFlag(MPQBlockEntry::Flags flag) const -> bool { return (static_cast<uint32_t>(flags) & static_cast<uint32_t>(flag)) != 0; }

auto MPQBlockEntry::getFilePosition() const -> uint32_t { return filePosition; }

auto MPQBlockEntry::getCompressedSize() const -> uint32_t { return compressedSize; }

auto MPQBlockEntry::getUncompressedSize() const -> uint32_t { return uncompressedSize; }

auto MPQBlockEntry::getFlags() const -> MPQBlockEntry::Flags { return flags; }

auto MPQBlockEntry::getEncryptionSeed() const -> uint32_t { return encryptionSeed; }

auto MPQBlockEntry::isEncrypted() const -> bool { return hasFlag(Flags::FILE_ENCRYPTED); }

auto MPQBlockEntry::isCompressed() const -> bool { return hasFlag(Flags::FILE_COMPRESS); }

auto MPQBlockEntry::isSingleUnit() const -> bool { return hasFlag(Flags::FILE_SINGLE_UNIT); }

auto MPQBlockEntry::isImploded() const -> bool { return hasFlag(Flags::FILE_IMPLODE); }

auto MPQBlockEntry::isPatched() const -> bool { return hasFlag(Flags::FILE_PATCH_FILE); }

auto MPQBlockEntry::isDeleted() const -> bool { return hasFlag(Flags::FILE_DELETE_MARKER); }

auto MPQBlockEntry::hasSectorCRC() const -> bool { return hasFlag(Flags::FILE_SECTOR_CRC); }

auto MPQBlockEntry::exists() const -> bool { return hasFlag(Flags::FILE_EXISTS); }

auto MPQBlockEntry::hasFixKey() const -> bool { return hasFlag(Flags::FILE_FIX_KEY); }

void MPQBlockEntry::calculateEncryptionSeed(const std::string_view name) {
    const auto extractedFileName = name.substr(name.find_last_of('/') + 1);
    const auto seed = Crypto::getInstance().hashString(extractedFileName, 3);
    encryptionSeed = (seed + filePosition) ^ uncompressedSize;
}

} // namespace Abyss::MPQ
