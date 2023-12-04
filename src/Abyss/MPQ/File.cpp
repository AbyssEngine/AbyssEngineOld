#include "File.h"

namespace Abyss::MPQ {

File::File(const std::filesystem::path &filePath) : path(filePath), file(filePath, std::ios::in | std::ios::binary), header() {
    this->path = std::filesystem::canonical(this->path);

    if (!file.is_open())
        throw std::runtime_error("Failed to open MPQ file: " + this->path.string());

    file.read(std::bit_cast<char *>(&header), sizeof(Header));

    if (header.magic != MPQ_MAGIC)
        throw std::runtime_error("Invalid MPQ file: " + this->path.string());

    if (header.headerSize != sizeof(Header))
        throw std::runtime_error("Invalid MPQ header size: " + std::to_string(header.headerSize));

    if (header.formatVersion != 0)
        throw std::runtime_error("Unsupported MPQ format _version: " + std::to_string(header.formatVersion));

    readHashTable();
    readBlockTable();
}

auto File::readHashTable() -> void {
    file.seekg(header.hashTableOffset);
    const auto hashData = Crypto::getInstance().decryptTable(file, header.hashTableEntries * 4, "(hash table)");
    hashes.clear();

    for (uint32_t tableEntryIdx = 0, idx = 0; tableEntryIdx < header.hashTableEntries; tableEntryIdx++, idx += 4) {
        const auto hashA = hashData[idx];
        const auto hashB = hashData[idx + 1];
        const auto locale = static_cast<uint16_t>(hashData[idx + 2] >> 16);
        const auto platform = static_cast<uint16_t>(hashData[idx + 2] & 0xFFFF);
        const auto blockIndex = hashData[idx + 3];
        HashEntry const hashEntry(hashA, hashB, locale, platform, blockIndex);
        hashes.emplace(hashEntry.Name64(), hashEntry);
    }
}

auto File::readBlockTable() -> void {
    file.seekg(header.blockTableOffset);
    const auto blockData = Crypto::getInstance().decryptTable(file, header.blockTableEntries * 4, "(block table)");
    blocks.clear();

    for (uint32_t tableEntryIdx = 0, idx = 0; tableEntryIdx < header.blockTableEntries; tableEntryIdx++, idx += 4) {
        auto filePosition = blockData[idx];
        auto compressedSize = blockData[idx + 1];
        auto uncompressedSize = blockData[idx + 2];
        auto flags = blockData[idx + 3];
        blocks.emplace_back(filePosition, compressedSize, uncompressedSize, flags);
    }
}

auto File::getHeader() const -> const Header & { return header; }

auto File::getFileBlockData(const std::string &fileName) -> MPQBlockEntry & {
    const auto hash = Crypto::getInstance().hashFileName(fileName);
    const auto hashEntry = hashes.find(hash);

    if (hashEntry == hashes.end()) {
        throw std::runtime_error("File not found in " + path.string() + ": " + fileName);
    }

    auto &blockEntry = blocks.at(hashEntry->second.getBlockIndex());

    if (!blockEntry.exists()) {
        throw std::runtime_error("File found but marked as non-existent in MPQ file: " + fileName);
    }

    if (blockEntry.hasFixKey()) {
        blockEntry.calculateEncryptionSeed(fileName);
    }

    return blockEntry;
}

auto File::getBlockOffsets(const MPQBlockEntry &blockEntry) -> std::vector<uint32_t> {
    file.seekg(blockEntry.getFilePosition());
    const auto vsize = (0x200 << header.blockSize);
    const auto blockPositionCount = ((blockEntry.getUncompressedSize() + vsize - 1) / vsize) + 1;
    std::vector<uint32_t> blockPositions(blockPositionCount);
    file.read(std::bit_cast<char *>(blockPositions.data()), static_cast<std::streamsize>(blockPositionCount * sizeof(uint32_t)));

    if (blockEntry.isEncrypted()) {
        Crypto::getInstance().decrypt(blockPositions, blockEntry.getEncryptionSeed() - 1);
        const auto blockPosSize = blockPositionCount << 2;
        if (blockPositions.at(0) != blockPosSize) {
            throw std::runtime_error("Decryption failure.");
        }
        if (blockPositions.at(1) > vsize + blockPosSize) {
            throw std::runtime_error("Decryption failure.");
        }
    }

    return blockPositions;
}

auto File::readData(const std::streampos &offset, const std::streamsize size) -> std::vector<std::byte> {
    std::vector<std::byte> data(size);
    file.seekg(offset);
    file.read(std::bit_cast<char *>(data.data()), size);
    return data;
}

auto File::hasFile(const std::string_view fileName) const -> bool {
    const auto hash = Crypto::getInstance().hashFileName(fileName);
    const auto hashEntry = hashes.find(hash);

    if (hashEntry == hashes.end()) {
        return false;
    }

    return blocks.at(hashEntry->second.getBlockIndex()).exists();
}

} // namespace Abyss::MPQ
