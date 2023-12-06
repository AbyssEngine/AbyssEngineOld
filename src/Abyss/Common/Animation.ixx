module;

#include <chrono>
#include <cstdint>
#include <string>

import Abyss.Common.Drawable;
import Abyss.DataTypes.Palette;
import Abyss.Enums.BlendMode;

export module Abyss.Common.Animation;

namespace Abyss::Common {

inline constexpr float AnimationFPS = 25.0;
inline constexpr float AnimationDivisor = 1.0 / 256.0;
inline constexpr float AnimationSpeedUnit = AnimationFPS * AnimationDivisor;

export template <Drawable T> class Animation {
    T _drawable;
    double _frameTime;
    uint32_t _frameIdx;
    double _animationSpeed;

  public:
    explicit Animation(std::string path) : _drawable(path), _frameTime(), _frameIdx(), _animationSpeed(0.5 * (AnimationSpeedUnit)) {}

    auto draw(int x, int y) -> void { _drawable.draw(_frameIdx, x, y); }

    auto update(const std::chrono::duration<double> deltaTime) -> void {
        _frameTime += deltaTime.count();
        const auto framesToAdd = static_cast<uint32_t>(_frameTime / _animationSpeed);
        _frameTime -= framesToAdd * _animationSpeed;
        _frameIdx += framesToAdd;
        _frameIdx %= _drawable.getFrameCount();
    }

    auto setPalette(const DataTypes::Palette &palette) -> void { _drawable.setPalette(palette); }

    auto setBlendMode(Enums::BlendMode blendMode) -> void { _drawable.setBlendMode(blendMode); }
};

} // namespace Abyss::Common
