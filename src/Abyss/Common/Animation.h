#pragma once

#include "Abyss/Concepts/Drawable.h"
#include <chrono>
#include <cstdint>
#include <string_view>

namespace Abyss::Common {

inline constexpr float AnimationFPS = 25.0;
inline constexpr float AnimationDivisor = 1.0 / 256.0;
inline constexpr float AnimationSpeedUnit = AnimationFPS * AnimationDivisor;

template <Concepts::Drawable T> class Animation {
    T _drawable;
    double _frameTime;
    uint32_t _frameIdx;
    double _animationSpeed;

  public:
    explicit Animation(std::string_view path) : _drawable(path), _frameTime(), _frameIdx(), _animationSpeed(0.5 * (AnimationSpeedUnit)) {}
    void draw(int x, int y) { _drawable.draw(_frameIdx, x, y); }

    void update(const std::chrono::duration<double> deltaTime) {
        _frameTime += deltaTime.count();
        const auto framesToAdd = static_cast<uint32_t>(_frameTime / _animationSpeed);
        _frameTime -= framesToAdd * _animationSpeed;
        _frameIdx += framesToAdd;
        _frameIdx %= _drawable.getFrameCount();
    }

    void setPalette(const Abyss::DataTypes::Palette &palette) { _drawable.setPalette(palette); }

    void setBlendMode(Enums::BlendMode blendMode) { _drawable.setBlendMode(blendMode); }
};

} // namespace Abyss::Common
