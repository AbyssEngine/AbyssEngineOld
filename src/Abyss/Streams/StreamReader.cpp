#include "StreamReader.h"

namespace Abyss::Streams {

StreamReader::StreamReader(FileSystem::InputStream &inputStream) : _inputStream(inputStream) {}

uint8_t StreamReader::readByte() const { return static_cast<uint8_t>(_inputStream.get()); }

void StreamReader::readBytes(std::span<uint8_t> data) const {
    if (data.empty()) {
        return;
    }
    _inputStream.read(reinterpret_cast<char *>(&data[0]), static_cast<long>(data.size()));
}

uint8_t StreamReader::readUInt8() { return ReadUnsigned<uint8_t>(); }

int8_t StreamReader::readInt8() { return static_cast<int8_t>(readUInt8()); }

uint16_t StreamReader::readUInt16() { return ReadUnsigned<uint16_t>(); }

int16_t StreamReader::readInt16() { return static_cast<int16_t>(readUInt16()); }

uint32_t StreamReader::readUInt32() { return ReadUnsigned<uint32_t>(); }

int32_t StreamReader::readInt32() { return static_cast<int32_t>(readUInt32()); }

uint64_t StreamReader::readUInt64() { return ReadUnsigned<uint64_t>(); }

int64_t StreamReader::readInt64() { return static_cast<int64_t>(readUInt64()); }

std::string StreamReader::readString() const {
    std::string result;
    char c;

    while ((c = static_cast<char>(_inputStream.get())) != '\0') {
        result.push_back(c);
    }

    return result;
}

} // namespace Abyss::Streams
