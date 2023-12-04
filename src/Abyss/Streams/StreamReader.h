#pragma once

#include "InputStream.h"
#include <span>
#include <string>

namespace Abyss::Streams {

class StreamReader {
  private:
    InputStream &inputStream;

  public:
    explicit StreamReader(InputStream &inputStream);

    [[nodiscard]] uint8_t ReadByte();
    void ReadBytes(std::span<uint8_t> data);
    template <typename T> [[nodiscard]] T ReadUnsigned();
    [[nodiscard]] auto ReadUInt8() -> uint8_t;
    [[nodiscard]] auto ReadInt8() -> int8_t;
    [[nodiscard]] uint16_t ReadUInt16();
    [[nodiscard]] int16_t ReadInt16();
    [[nodiscard]] uint32_t ReadUInt32();
    [[nodiscard]] int32_t ReadInt32();
    [[nodiscard]] uint64_t ReadUInt64();
    [[nodiscard]] int64_t ReadInt64();
    [[nodiscard]] std::string ReadString();
};

} // namespace Abyss::Streams
