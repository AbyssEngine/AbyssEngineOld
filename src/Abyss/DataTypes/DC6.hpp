#pragma once

#include <Abyss/Common/Animation.hpp>
#include <Abyss/DataTypes/DC6Frame.hpp>
#include <Abyss/DataTypes/Palette.hpp>
#include <Abyss/Enums/BlendMode.hpp>
#include <Abyss/Streams/InputStream.h>
#include <SDL2/SDL.h>
#include <array>
#include <vector>

namespace Abyss::DataTypes {

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
    auto setPalette(const DataTypes::Palette &palette) -> void;
    auto draw(uint32_t frameIdx, int x, int y) const -> void;
    auto draw(uint32_t frameIdx, int x, int y, int framesX, int framesY) const -> void;
    auto setBlendMode(Enums::BlendMode blendMode) -> void;
    auto getFrameSize(uint32_t frameIdx, int &frameWidth, int &frameHeight) const -> void;
    auto getFrameOffset(uint32_t frameIdx, int &offsetX, int &offsetY) const -> void;
};

typedef Common::Animation<DC6> DC6Animation;

} // namespace Abyss::DataTypes
