#pragma once

#include "Abyss/Streams/StreamReader.h"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace Abyss::DataTypes {

inline constexpr std::byte DC6EndOfScanline = static_cast<const std::byte>(0x80);
inline constexpr std::byte DC6MaxRunLength = static_cast<const std::byte>(0x7F);
inline constexpr int DC6TerminationSize = 4;
inline constexpr int DC6TerminatorSize = 3;

class DC6Frame {
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
    explicit DC6Frame(Streams::StreamReader& stream);
    [[nodiscard]] uint32_t getFlipped() const;
    [[nodiscard]] uint32_t getWidth() const;
    [[nodiscard]] uint32_t getHeight() const;
    [[nodiscard]] int32_t getXOffset() const;
    [[nodiscard]] int32_t getYOffset() const;
    [[nodiscard]] uint32_t getUnknown() const;
    [[nodiscard]] uint32_t getNextBlock() const;
    [[nodiscard]] uint32_t getLength() const;
    [[nodiscard]] const std::vector<std::byte>& getFrameData() const;
    [[nodiscard]] const std::vector<std::byte>& getTerminator() const;
};

} // namespace Abyss::DataTypes
