#ifndef LIBABYSS_PALETTE_H
#define LIBABYSS_PALETTE_H

#include <cstdint>
#include <vector>
#include <istream>
#include "inputstream.h"

namespace LibAbyss {

    struct PaletteColor {
        PaletteColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
            Red = r;
            Green = g;
            Blue = b;
            Alpha = a;
        }
        PaletteColor(uint8_t r, uint8_t g, uint8_t b) {
            Red = r;
            Green = g;
            Blue = b;
            Alpha = 0xFF;
        }
        explicit PaletteColor(uint8_t c) {
            Red = c;
            Green = c;
            Blue = c;
            Alpha = 0xFF;
        }
        PaletteColor() {
            Red = 0;
            Green = 0;
            Blue = 0;
            Alpha = 0;
        }

        uint8_t Red;
        uint8_t Green;
        uint8_t Blue;
        uint8_t Alpha;

        explicit operator uint32_t() const {
            return ((uint32_t)Red) | ((uint32_t)Green << 8) | ((uint32_t)Blue << 16) | ((uint32_t)Alpha << 24);
        }
    };

    class Palette {
    public:
        Palette(std::istream &stream, bool isDat);

        std::vector<PaletteColor> BasePalette;

        std::vector<std::vector<uint8_t>> LightLevelVariations = std::vector<std::vector<uint8_t>>();
        std::vector<std::vector<uint8_t>> InvColorVariations = std::vector<std::vector<uint8_t>>();
        std::vector<uint8_t> SelectedUnitShift = std::vector<uint8_t>();
        std::vector<std::vector<std::vector<uint8_t>>> AlphaBlend = std::vector<std::vector<std::vector<uint8_t>>>();
        std::vector<std::vector<uint8_t>> AdditiveBlend = std::vector<std::vector<uint8_t>>();
        std::vector<std::vector<uint8_t>> MultiplicativeBlend = std::vector<std::vector<uint8_t>>();
        std::vector<std::vector<uint8_t>> HueVariations = std::vector<std::vector<uint8_t>>();
        std::vector<uint8_t> RedTones = std::vector<uint8_t>();
        std::vector<uint8_t> GreenTones = std::vector<uint8_t>();
        std::vector<uint8_t> BlueTones = std::vector<uint8_t>();
        std::vector<std::vector<uint8_t>> UnknownVariations = std::vector<std::vector<uint8_t>>();
        std::vector<std::vector<uint8_t>> MaxComponentBlend = std::vector<std::vector<uint8_t>>();
        std::vector<uint8_t> DarkenedColorShift = std::vector<uint8_t>();

        std::vector<PaletteColor> TextColors = std::vector<PaletteColor>();
        std::vector<std::vector<uint8_t>> TextColorShifts = std::vector<std::vector<uint8_t>>();
    private:
        static std::vector<PaletteColor> DecodeColors(std::istream &stream, uint8_t colorBytes, int numColors);
        static std::vector<std::vector<uint8_t>> LoadTransformMulti(std::istream &stream, int variations);

        static std::vector<uint8_t> LoadTransformSingle(std::istream &stream);
    };

}

#endif //LIBABYSS_PALETTE_H
