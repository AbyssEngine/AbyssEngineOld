#pragma once

#include <Abyss/DataTypes/Palette.h>
#include <Abyss/Enums/BlendMode.h>
#include <chrono>

namespace Abyss::Common {

template <typename T>
concept Drawable =
    requires(T t, int x, int y, uint32_t frameIdx, const DataTypes::Palette &palette, Enums::BlendMode blendMode, int &frameWidth, int &frameHeight) {
        { T(std::string_view{}) } -> std::same_as<T>;
        { t.draw(frameIdx, x, y) } -> std::same_as<void>;
        { t.setPalette(palette) } -> std::same_as<void>;
        { t.setBlendMode(blendMode) } -> std::same_as<void>;
        { t.getFrameCount() } -> std::same_as<uint32_t>;
        { t.getFrameSize(frameIdx, frameWidth, frameHeight) } -> std::same_as<void>;
    };
} // namespace Abyss::Common
