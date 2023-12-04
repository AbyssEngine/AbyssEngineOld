#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace Abyss::MPQ {

class Crypto {
    std::array<uint32_t, 0x500> cryptTable;
    Crypto();

  public:
    static auto getInstance() -> Crypto &;

    [[nodiscard]] auto hashString(std::string_view key, uint32_t hashType) const -> uint32_t;

    [[nodiscard]] auto hashFileName(std::string_view fileName) const -> uint64_t;

    auto initializeCryptTable() -> void;

    auto decrypt(std::istream &input, std::ostream &output, uint32_t seed) const -> void;

    auto decrypt(std::vector<uint32_t> &input, uint32_t seed) const -> void;

    auto decrypt(std::vector<std::byte> &input, uint32_t seed) const -> void;

    auto decryptTable(std::ifstream &file, uint32_t size, std::string_view name) const -> std::vector<uint32_t>;
};

} // namespace Abyss::MPQ
