#pragma once

#include <Abyss/MPQ/BlockEntry.h>
#include <Abyss/MPQ/HashEntry.h>
#include <Abyss/MPQ/Header.h>
#include <array>
#include <coroutine>
#include <filesystem>
#include <fstream>
#include <map>
#include <vector>

namespace Abyss::MPQ {

class File {
  private:
    std::filesystem::path path;
    std::ifstream file;
    Header header;
    std::map<uint64_t, HashEntry> hashes;
    std::vector<MPQBlockEntry> blocks;

    static constexpr std::array<char, 4> MPQ_MAGIC = {'M', 'P', 'Q', 0x1A};
    auto readHashTable() -> void;
    auto readBlockTable() -> void;

  public:
    explicit File(const std::filesystem::path &filePath);
    File(const File &) = delete;
    File(File &&) noexcept = delete;
    auto operator=(const File &) = delete;
    auto operator=(File &&) noexcept = delete;

    [[nodiscard]] auto getHeader() const -> const Header &;
    auto getFileBlockData(const std::string &fileName) -> MPQBlockEntry &;
    auto getBlockOffsets(const MPQBlockEntry &blockEntry) -> std::vector<uint32_t>;
    auto readData(const std::streampos &offset, std::streamsize size) -> std::vector<std::byte>;
    [[nodiscard]] auto hasFile(std::string_view fileName) const -> bool;
};

} // namespace Abyss::MPQ
