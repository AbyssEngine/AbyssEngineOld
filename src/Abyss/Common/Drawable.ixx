module;

#include <cstdint>
#include <string>

import Abyss.Enums.BlendMode;
import Abyss.DataTypes.Palette;

export module Abyss.Common.Drawable;

export template <typename T>
concept Drawable = requires(T t, int x, int y, uint32_t frameIdx, const Abyss::DataTypes::Palette &palette, Abyss::Enums::BlendMode blendMode, int &frameWidth,
                            int &frameHeight, int &offsetX, int &offsetY) {
    { T(std::string_view{}) } -> std::same_as<T>;
    { t.draw(frameIdx, x, y) } -> std::same_as<void>;
    { t.setPalette(palette) } -> std::same_as<void>;
    { t.setBlendMode(blendMode) } -> std::same_as<void>;
    { t.getFrameCount() } -> std::same_as<uint32_t>;
    { t.getFrameSize(frameIdx, frameWidth, frameHeight) } -> std::same_as<void>;
    { t.getFrameOffset(frameIdx, offsetX, offsetY) } -> std::same_as<void>;
};
