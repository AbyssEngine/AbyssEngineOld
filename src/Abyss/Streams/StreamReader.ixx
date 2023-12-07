module;

#include <span>
#include <string>

export module Abyss.Streams.StreamReader;

import Abyss.FileSystem.InputStream;

namespace Abyss::Streams {

export class StreamReader {
    FileSystem::InputStream &_inputStream;

  public:
    explicit StreamReader(FileSystem::InputStream &inputStream) : _inputStream(inputStream) {}

    [[nodiscard]] auto readByte() const -> uint8_t { return static_cast<uint8_t>(_inputStream.get()); }

    auto readBytes(std::span<uint8_t> data) const -> void {
        if (data.empty()) {
            return;
        }
        _inputStream.read(reinterpret_cast<char *>(&data[0]), static_cast<long>(data.size()));
    }

    [[nodiscard]] auto readUInt8() -> uint8_t { return ReadUnsigned<uint8_t>(); }

    [[nodiscard]] auto readInt8() -> int8_t { return static_cast<int8_t>(readUInt8()); }

    [[nodiscard]] auto readUInt16() -> uint16_t { return ReadUnsigned<uint16_t>(); }

    [[nodiscard]] auto readInt16() -> int16_t { return static_cast<int16_t>(readUInt16()); }

    [[nodiscard]] auto readUInt32() -> uint32_t { return ReadUnsigned<uint32_t>(); }

    [[nodiscard]] auto readInt32() -> int32_t { return static_cast<int32_t>(readUInt32()); }

    [[nodiscard]] auto readUInt64() -> uint64_t { return ReadUnsigned<uint64_t>(); }

    [[nodiscard]] auto readInt64() -> int64_t { return static_cast<int64_t>(readUInt64()); }

    [[nodiscard]] auto readString() const -> std::string {
        std::string result;
        char c;

        while ((c = static_cast<char>(_inputStream.get())) != '\0') {
            result.push_back(c);
        }

        return result;
    }

    template <typename T> T ReadUnsigned() {
        T result = 0;

        for (auto i = 0; i < static_cast<int>(sizeof(T)); i++) {
            result |= static_cast<T>(readByte()) << 8 * i;
        }

        return result;
    }
};

} // namespace Abyss::Streams
