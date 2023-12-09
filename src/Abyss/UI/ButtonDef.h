#pragma once

#include "Abyss/DataTypes/Palette.h"

#include <SDL2/SDL.h>
#include <cstdint>
#include <string>

namespace Abyss::UI {

struct ButtonDef {
    std::string name;
    std::string resourceName;
    DataTypes::Palette palette;
    std::string font;
    std::string clickSound;
    SDL_Rect clickableRect;
    struct {
        uint8_t x;
        uint8_t y;
    } segments;
    struct {
        int8_t base;
        int8_t pressed;
        int8_t disabled;
    } frames;
};

} // namespace Abyss::UI
