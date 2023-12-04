#pragma once

#include <Abyss/Common/Animation.h>
#include <Abyss/Enums/BlendMode.h>
#include <Abyss/Streams/InputStream.h>
#include <SDL2/SDL.h>
#include <array>
#include <vector>

namespace Abyss::DataTypes {

constexpr std::byte DC6EndOfScanline = static_cast<const std::byte>(0x80);
constexpr std::byte DC6MaxRunLength = static_cast<const std::byte>(0x7F);
constexpr int DC6TerminationSize = 4;
constexpr int DC6TerminatorSize = 3;

class DC6Frame {
    uint32_t _flipped;
    uint32_t _width;
    uint32_t _height;
    int32_t _xOffset;
    int32_t _yOffset;
    uint32_t _unknown;
    uint32_t _nextBlock;
    uint32_t _length;
    std::vector<std::byte> _frameData;
    std::vector<std::byte> _terminator;

  public:
    explicit DC6Frame(Streams::InputStream &stream);
    [[nodiscard]] auto getFlipped() const -> uint32_t;
    [[nodiscard]] auto getWidth() const -> uint32_t;
    [[nodiscard]] auto getHeight() const -> uint32_t;
    [[nodiscard]] auto getXOffset() const -> int32_t;
    [[nodiscard]] auto getYOffset() const -> int32_t;
    [[nodiscard]] auto getUnknown() const -> uint32_t;
    [[nodiscard]] auto getNextBlock() const -> uint32_t;
    [[nodiscard]] auto getLength() const -> uint32_t;
    [[nodiscard]] auto getFrameData() const -> std::vector<std::byte>;
    [[nodiscard]] auto getTerminator() const -> std::vector<std::byte>;
};

class DC6 {
    uint32_t _version;
    uint32_t _flags;
    uint32_t _encoding;
    std::array<uint8_t, DC6TerminationSize> _termination;
    uint32_t _directions;
    uint32_t _framesPerDirection;
    std::vector<uint32_t> _framePointers;
    std::vector<DC6Frame> _frames;
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> _texture;
    std::vector<SDL_Rect> _frameRects;
    Enums::BlendMode _blendMode;

  public:
    explicit DC6(std::string_view path);
    [[nodiscard]] auto getVersion() const -> uint32_t;
    [[nodiscard]] auto getFlags() const -> uint32_t;
    [[nodiscard]] auto getEncoding() const -> uint32_t;
    [[nodiscard]] auto getTermination() const -> std::array<uint8_t, 4>;
    [[nodiscard]] auto getDirections() const -> uint32_t;
    [[nodiscard]] auto getFramesPerDirection() const -> uint32_t;
    [[nodiscard]] auto getFramePointers() const -> std::vector<uint32_t>;
    [[nodiscard]] auto getFrameCount() const -> uint32_t;
    auto setPalette(const Palette &palette) -> void;
    auto draw(uint32_t frameIdx, int x, int y) const -> void;
    auto draw(uint32_t frameIdx, int x, int y, int framesX, int framesY) const -> void;
    auto setBlendMode(Enums::BlendMode blendMode) -> void;
    auto getFrameSize(uint32_t frameIdx, int &frameWidth, int &frameHeight) const -> void;
};

typedef Common::Animation<DC6> DC6Animation;

} // namespace Abyss::DataTypes
