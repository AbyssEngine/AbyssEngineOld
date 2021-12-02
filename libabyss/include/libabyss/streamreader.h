#ifndef LIBABYSS_STREAMREADER_H
#define LIBABYSS_STREAMREADER_H

#include "inputstream.h"
#include <cstdint>
#include <concepts>
#include <span>

namespace LibAbyss {
class StreamReader {
  public:
    explicit StreamReader(InputStream &inputStream);

    uint8_t ReadByte();
    void ReadBytes(std::span<uint8_t> data);

    template <std::unsigned_integral T> T ReadUnsigned() {
        T result = 0;

        for (auto i = 0; i < sizeof(T); i++)
            result |= ((T)ReadByte()) << (8 * i);

        return result;
    }

    uint8_t ReadUInt8() { return ReadUnsigned<uint8_t>(); }
    uint8_t ReadInt8() { return (int8_t)ReadUInt8(); }

    uint16_t ReadUInt16() { return ReadUnsigned<uint16_t>(); }
    int16_t ReadInt16() { return (int16_t)ReadUInt16(); }

    uint32_t ReadUInt32() { return ReadUnsigned<uint32_t>(); }
    int32_t ReadInt32() { return (int32_t)ReadUInt32(); }

    uint64_t ReadUInt64() { return ReadUnsigned<uint64_t>(); }
    int64_t ReadInt64() { return (int64_t)ReadUInt64(); }

  private:
    InputStream &_inputStream;
};
} // namespace LibAbyss

#endif // LIBABYSS_STREAMREADER_H
