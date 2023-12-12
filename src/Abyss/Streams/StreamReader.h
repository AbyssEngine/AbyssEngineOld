#pragma once

#include "Abyss/FileSystem/InputStream.h"
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <concepts>

namespace Abyss::Streams {

class StreamReader {
    FileSystem::InputStream &_inputStream;

  public:
    explicit StreamReader(FileSystem::InputStream &inputStream);
    [[nodiscard]] uint8_t readByte() const;
    void readBytes(std::span<uint8_t> data) const;
    void readBytes(std::span<int8_t> data) const;
    void readBytes(std::span<char> data) const;
    void readBytes(std::span<std::byte> data) const;
    [[nodiscard]] uint8_t readUInt8();
    [[nodiscard]] int8_t readInt8();
    [[nodiscard]] uint16_t readUInt16();
    [[nodiscard]] int16_t readInt16();
    [[nodiscard]] uint32_t readUInt32();
    [[nodiscard]] int32_t readInt32();
    [[nodiscard]] uint64_t readUInt64();
    [[nodiscard]] int64_t readInt64();

    // Reads until 0 byte.
    [[nodiscard]] std::string readString();

    template <std::unsigned_integral T> T readUnsigned() {
        T result = 0;

        for (auto i = 0; i < static_cast<int>(sizeof(T)); i++) {
            result |= static_cast<T>(readByte()) << (8 * i);
        }

        return result;
    }

    void skip(int64_t numBytes);
    void seek(int64_t numBytes);
};

} // namespace Abyss::Streams
