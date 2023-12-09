#pragma once

#include "Abyss/FileSystem/InputStream.h"
#include <cstdint>
#include <span>
#include <string>

namespace Abyss::Streams {

class StreamReader {
    FileSystem::InputStream &_inputStream;

  public:
    explicit StreamReader(FileSystem::InputStream &inputStream);
    [[nodiscard]] uint8_t readByte() const;
    void readBytes(std::span<uint8_t> data) const;
    [[nodiscard]] uint8_t readUInt8();
    [[nodiscard]] int8_t readInt8();
    [[nodiscard]] uint16_t readUInt16();
    [[nodiscard]] int16_t readInt16();
    [[nodiscard]] uint32_t readUInt32();
    [[nodiscard]] int32_t readInt32();
    [[nodiscard]] uint64_t readUInt64();
    [[nodiscard]] int64_t readInt64();
    [[nodiscard]] std::string readString() const;
    template <typename T> T ReadUnsigned() {
        T result = 0;

        for (auto i = 0; i < static_cast<int>(sizeof(T)); i++) {
            result |= static_cast<T>(readByte()) << 8 * i;
        }

        return result;
    }
};

} // namespace Abyss::Streams
