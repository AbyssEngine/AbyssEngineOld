#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace Abyss::MPQ {

class Crypto {
    std::array<uint32_t, 0x500> cryptTable;
    Crypto() : cryptTable({}) { initializeCryptTable(); }

  public:
    [[nodiscard]] static auto getInstance() -> Crypto & {
        static Crypto instance;
        return instance;
    }

    [[nodiscard]] auto hashString(const std::string_view key, const uint32_t hashType) const -> uint32_t {
        uint32_t seed1 = 0x7FED7FED;
        uint32_t seed2 = 0xEEEEEEEE;

        for (auto character : key) {
            character = static_cast<char>(toupper(character));
            seed1 = cryptTable.at((hashType * 0x100) + static_cast<uint32_t>(character)) ^ (seed1 + seed2);
            seed2 = static_cast<uint32_t>(character) + seed1 + seed2 + (seed2 << 5) + 3;
        }

        return seed1;
    }

    [[nodiscard]] auto hashFileName(const std::string_view fileName) const -> uint64_t {
        auto str = std::string(fileName);
        std::ranges::replace(str, '/', '\\');
        if (str[0] == '\\') {
            str.erase(0, 1);
        }
        std::ranges::transform(str, str.begin(), ::tolower);

        const auto hashA = hashString(str, 1);
        const auto hashB = hashString(str, 2);
        return (static_cast<uint64_t>(hashB) << 32) | static_cast<uint64_t>(hashA);
    }

    auto initializeCryptTable() -> void {
        uint32_t seed = 0x00100001;

        for (uint32_t index1 = 0; index1 < 0x100; index1++) {
            for (uint32_t index2 = index1, i = 0; i < 5; i++, index2 += 0x100) {
                seed = (seed * 125 + 3) % 0x2AAAAB;
                const auto temp1 = (seed & 0xFFFF) << 0x10;
                seed = (seed * 125 + 3) % 0x2AAAAB;
                const auto temp2 = (seed & 0xFFFF);
                cryptTable.at(index2) = (temp1 | temp2);
            }
        }
    }

    auto decrypt(std::istream &input, std::ostream &output, uint32_t seed) const -> void {
        uint32_t seed2 = 0xEEEEEEEE;
        uint32_t value = 0;

        while (input.read(std::bit_cast<char *>(&value), sizeof(uint32_t))) {
            seed2 += cryptTable.at(0x400 + (seed & 0xFF));
            value ^= seed + seed2;

            seed = ((~seed << 21) + 0x11111111) | (seed >> 11);
            seed2 = value + seed2 + (seed2 << 5) + 3;

            output.write(std::bit_cast<const char *>(&value), sizeof(uint32_t));
        }
    }

    auto decrypt(std::vector<uint32_t> &input, uint32_t seed) const -> void {
        uint32_t seed2 = 0xEEEEEEEE;

        for (auto &value : input) {
            seed2 += cryptTable.at(0x400 + (seed & 0xFF));
            value ^= seed + seed2;

            seed = ((~seed << 21) + 0x11111111) | (seed >> 11);
            seed2 = value + seed2 + (seed2 << 5) + 3;
        }
    }

    auto decrypt(std::vector<std::byte> &input, uint32_t seed) const -> void {
        uint32_t seed2 = 0xEEEEEEEE;

        // Ensure the input size is a multiple of 4 bytes
        const size_t inputSize = input.size();
        const size_t numUint32s = inputSize / 4;

        for (size_t i = 0; i < numUint32s; ++i) {
            uint32_t byteValue = 0;

            // Combine 4 bytes to form a uint32_t
            for (int j = 0; j < 4; ++j) {
                byteValue |= static_cast<uint32_t>(input[i * 4 + j]) << (j * 8);
            }

            seed2 += cryptTable.at(0x400 + (seed & 0xFF));
            byteValue ^= seed + seed2;

            seed = ((~seed << 21) + 0x11111111) | (seed >> 11);
            seed2 = byteValue + seed2 + (seed2 << 5) + 3;

            // Split the modified uint32_t back into 4 bytes
            for (int j = 0; j < 4; ++j) {
                input[i * 4 + j] = static_cast<std::byte>((byteValue >> (j * 8)) & 0xFF);
            }
        }
    }

    auto decryptTable(std::ifstream &file, const uint32_t size, const std::string_view name) const -> std::vector<uint32_t> {
        uint32_t seed = hashString(name, 3);
        uint32_t seed2 = 0xEEEEEEEE;

        std::vector<uint32_t> table(size); // Initialize the table with the required size
        std::vector<uint8_t> buf(4);       // Buffer to read 4 bytes at a time

        for (uint32_t i = 0; i < size; i++) {
            seed2 += cryptTable.at(0x400 + (seed & 0xFF));

            // Read 4 bytes from the file
            file.read(std::bit_cast<char *>(buf.data()), 4);

            if (file.fail()) {
                throw std::runtime_error("Failed to read " + std::string(name));
            }

            // Convert the buffer to a 32-bit integer using Little Endian byte order
            uint32_t result = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
            result ^= seed + seed2;

            seed = ((~seed << 21) + 0x11111111) | (seed >> 11);
            seed2 = result + seed2 + (seed2 << 5) + 3;

            table[i] = result;
        }

        return table;
    }
};

} // namespace Abyss::MPQ
