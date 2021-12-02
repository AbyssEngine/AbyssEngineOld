#include "libabyss/palette.h"
#include <string>

namespace {
const int NUM_PALETTE_COLORS = 256;
const int NUM_TEXT_COLORS = 13;
const int LIGHT_LEVEL_VARIATIONS = 32;
const int INV_COLOR_VARIATIONS = 16;
const int ALPHA_BLEND_COARSE = 3;
const int ALPHA_BLEND_FINE = 256;
const int ADDITIVE_BLENDS = 256;
const int MULTIPLY_BLENDS = 256;
const int HUE_VARIATIONS = 111;
const int UNKNOWN_VARIATIONS = 14;
const int COMPONENT_BLENDS = 256;
const int TEXT_SHIFTS = 13;
} // namespace

LibAbyss::Palette::Palette(std::istream &stream, bool isDat) {
    if (isDat) {
        BasePalette = DecodeColors(stream, 3, NUM_PALETTE_COLORS);
        return;
    }

    BasePalette = DecodeColors(stream, 4, NUM_PALETTE_COLORS);
    LightLevelVariations = LoadTransformMulti(stream, LIGHT_LEVEL_VARIATIONS);
    InvColorVariations = LoadTransformMulti(stream, INV_COLOR_VARIATIONS);
    SelectedUnitShift = LoadTransformSingle(stream);
    for (auto blendIdx = 0; blendIdx < ALPHA_BLEND_COARSE; blendIdx++)
        AlphaBlend.push_back(LoadTransformMulti(stream, ALPHA_BLEND_FINE));
    AdditiveBlend = LoadTransformMulti(stream, ADDITIVE_BLENDS);
    MultiplicativeBlend = LoadTransformMulti(stream, MULTIPLY_BLENDS);
    HueVariations = LoadTransformMulti(stream, HUE_VARIATIONS);
    RedTones = LoadTransformSingle(stream);
    GreenTones = LoadTransformSingle(stream);
    BlueTones = LoadTransformSingle(stream);
    UnknownVariations = LoadTransformMulti(stream, UNKNOWN_VARIATIONS);
    MaxComponentBlend = LoadTransformMulti(stream, COMPONENT_BLENDS);
    DarkenedColorShift = LoadTransformSingle(stream);
    TextColors = DecodeColors(stream, 3, TEXT_SHIFTS);
    TextColorShifts = LoadTransformMulti(stream, TEXT_SHIFTS);
}

std::vector<LibAbyss::PaletteColor> LibAbyss::Palette::DecodeColors(std::istream &stream, uint8_t colorBytes, int numColor) {
    std::vector<PaletteColor> result;

    if (numColor < 1)
        throw std::runtime_error("Unexpected number of colors: " + std::to_string(numColor));

    result.reserve(numColor);
    for (auto idx = 0; idx < numColor; idx++) {
        switch (colorBytes) {
        case 1: {
            char b;
            stream.read(&b, 1);
            result.emplace_back(b);
            continue;
        }
        case 3: {
            char b[3];
            stream.read(b, 3);
            result.emplace_back(b[0], b[1], b[2]);
            continue;
        }
        case 4: {
            char b[4];
            stream.read(b, 4);
            result.emplace_back(b[0], b[1], b[2], 0xFF);
            continue;
        }
        default:
            throw std::runtime_error("Invalid number of color bytes: " + std::to_string(colorBytes));
        }
    }

    result[0].Alpha = 0;

    return result;
}

std::vector<std::vector<uint8_t>> LibAbyss::Palette::LoadTransformMulti(std::istream &stream, int variations) {
    std::vector<std::vector<uint8_t>> result;
    result.reserve(variations);

    for (auto variationIdx = 0; variationIdx < variations; variationIdx++)
        result.push_back(LoadTransformSingle(stream));

    return result;
}

std::vector<uint8_t> LibAbyss::Palette::LoadTransformSingle(std::istream &stream) {
    std::vector<uint8_t> result(NUM_PALETTE_COLORS);
    stream.read((char *)result.data(), NUM_PALETTE_COLORS);
    return result;
}
