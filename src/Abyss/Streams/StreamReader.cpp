#include "StreamReader.h"

namespace Abyss::Streams {

StreamReader::StreamReader(InputStream &inputStream) : inputStream(inputStream) {}

uint8_t StreamReader::ReadByte() { return static_cast<uint8_t>(inputStream.get()); }

void StreamReader::ReadBytes(std::span<uint8_t> data) {
    if (data.empty()) {
        return;
    }
    inputStream.read(reinterpret_cast<char *>(&data[0]), (long)data.size());
}

auto StreamReader::ReadUInt8() -> uint8_t { return ReadUnsigned<uint8_t>(); }

auto StreamReader::ReadInt8() -> int8_t { return (int8_t)ReadUInt8(); }

uint16_t StreamReader::ReadUInt16() { return ReadUnsigned<uint16_t>(); }

int16_t StreamReader::ReadInt16() { return (int16_t)ReadUInt16(); }

uint32_t StreamReader::ReadUInt32() { return ReadUnsigned<uint32_t>(); }

int32_t StreamReader::ReadInt32() { return (int32_t)ReadUInt32(); }

uint64_t StreamReader::ReadUInt64() { return ReadUnsigned<uint64_t>(); }

int64_t StreamReader::ReadInt64() { return (int64_t)ReadUInt64(); }

std::string StreamReader::ReadString() {
    std::string result;
    char c;

    while ((c = (char)inputStream.get()) != '\0') {
        result.push_back(c);
    }

    return result;
}

template <typename T> T StreamReader::ReadUnsigned() {
    T result = 0;

    for (auto i = 0; i < (int)sizeof(T); i++) {
        result |= ((T)ReadByte()) << (8 * i);
    }

    return result;
}

} // namespace Abyss::Streams
