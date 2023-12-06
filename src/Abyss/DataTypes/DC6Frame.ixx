module;

#include <bit>
#include <cstddef>
#include <cstdint>
#include <vector>

import Abyss.Streams.InputStream;

export module Abyss.DataTypes.DC6Frame;

namespace Abyss::DataTypes {

export inline constexpr std::byte DC6EndOfScanline = static_cast<const std::byte>(0x80);
export inline constexpr std::byte DC6MaxRunLength = static_cast<const std::byte>(0x7F);
export inline constexpr int DC6TerminationSize = 4;
export inline constexpr int DC6TerminatorSize = 3;

export class DC6Frame {
    uint32_t _flipped{};
    uint32_t _width{};
    uint32_t _height{};
    int32_t _xOffset{};
    int32_t _yOffset{};
    uint32_t _unknown{};
    uint32_t _nextBlock{};
    uint32_t _length{};
    std::vector<std::byte> _frameData{};
    std::vector<std::byte> _terminator{};

  public:
    explicit DC6Frame(Streams::InputStream &stream) {
        stream.read(reinterpret_cast<char *>(&_flipped), sizeof(_flipped));
        stream.read(reinterpret_cast<char *>(&_width), sizeof(_width));
        stream.read(reinterpret_cast<char *>(&_height), sizeof(_height));
        stream.read(reinterpret_cast<char *>(&_xOffset), sizeof(_xOffset));
        stream.read(reinterpret_cast<char *>(&_yOffset), sizeof(_yOffset));
        stream.read(reinterpret_cast<char *>(&_unknown), sizeof(_unknown));
        stream.read(reinterpret_cast<char *>(&_nextBlock), sizeof(_nextBlock));
        stream.read(reinterpret_cast<char *>(&_length), sizeof(_length));

        _frameData.resize(_length);
        stream.read(std::bit_cast<char *>(_frameData.data()), _length);

        _terminator.resize(DC6TerminatorSize);
        stream.read(std::bit_cast<char *>(_terminator.data()), DC6TerminatorSize);
    }

    [[nodiscard]] auto getFlipped() const -> uint32_t { return _flipped; }

    [[nodiscard]] auto getWidth() const -> uint32_t { return _width; }

    [[nodiscard]] auto getHeight() const -> uint32_t { return _height; }

    [[nodiscard]] auto getXOffset() const -> int32_t { return _xOffset; }

    [[nodiscard]] auto getYOffset() const -> int32_t { return _yOffset; }

    [[nodiscard]] auto getUnknown() const -> uint32_t { return _unknown; }

    [[nodiscard]] auto getNextBlock() const -> uint32_t { return _nextBlock; }

    [[nodiscard]] auto getLength() const -> uint32_t { return _length; }

    [[nodiscard]] auto getFrameData() const -> std::vector<std::byte> { return _frameData; }

    [[nodiscard]] auto getTerminator() const -> std::vector<std::byte> { return _terminator; }
};

} // namespace Abyss::DataTypes
