#pragma once

#include "Abyss/DataTypes/Palette.h"
#include "Abyss/Enums/BlendMode.h"

#include <cstdint>
#include <string_view>

namespace Abyss::Concepts {

template <typename T>
concept Drawable = requires(T t, int x, int y, uint32_t frameIdx, const DataTypes::Palette &palette, Enums::BlendMode blendMode, int &frameWidth,
                            int &frameHeight, int &offsetX, int &offsetY) {
    { T(std::string_view{}) } -> std::same_as<T>;
    { t.draw(frameIdx, x, y) } -> std::same_as<void>;
    { t.setPalette(palette) } -> std::same_as<void>;
    { t.setBlendMode(blendMode) } -> std::same_as<void>;
    { t.getFrameCount() } -> std::same_as<uint32_t>;
    { t.getFrameSize(frameIdx, frameWidth, frameHeight) } -> std::same_as<void>;
    { t.getFrameOffset(frameIdx, offsetX, offsetY) } -> std::same_as<void>;
};

} // namespace Abyss::Concepts
