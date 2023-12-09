#pragma once

#include "Abyss/Common/Animation.h"
#include "Abyss/Enums/BlendMode.h"
#include "DC6Frame.h"
#include "Palette.h"

#include <SDL2/SDL.h>
#include <array>
#include <vector>

namespace Abyss::DataTypes {

class DC6 {
    uint32_t _version;
    uint32_t _flags;
    uint32_t _encoding;
    std::array<uint8_t, DC6TerminationSize> _termination{};
    uint32_t _directions;
    uint32_t _framesPerDirection;
    std::vector<uint32_t> _framePointers;
    std::vector<DC6Frame> _frames{};
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> _texture;
    std::vector<SDL_Rect> _frameRects;
    Enums::BlendMode _blendMode{};

  public:
    explicit DC6(std::string_view path);
    DC6(std::string_view path, const Palette &palette);
    [[nodiscard]] uint32_t getVersion() const;
    [[nodiscard]] uint32_t getFlags() const;
    [[nodiscard]] uint32_t getEncoding() const;
    [[nodiscard]] std::array<uint8_t, DC6TerminationSize> getTermination() const;
    [[nodiscard]] uint32_t getDirections() const;
    [[nodiscard]] uint32_t getFramesPerDirection() const;
    [[nodiscard]] std::vector<uint32_t> getFramePointers() const;
    [[nodiscard]] uint32_t getFrameCount() const;
    void setPalette(const Palette &palette);
    void draw(uint32_t frameIdx, int x, int y) const;
    void draw(uint32_t frameIdx, int x, int y, int framesX, int framesY) const;
    void setBlendMode(Enums::BlendMode blendMode);
    void getFrameSize(uint32_t frameIdx, int &frameWidth, int &frameHeight) const;
    void getFrameOffset(uint32_t frameIdx, int &offsetX, int &offsetY) const;
};

typedef Common::Animation<DC6> DC6Animation;

} // namespace Abyss::DataTypes
