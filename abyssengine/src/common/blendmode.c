/**
 * Copyright (C) 2021 Tim Sarbin
 * This file is part of AbyssEngine <https://github.com/AbyssEngine>.
 *
 * AbyssEngine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AbyssEngine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with AbyssEngine.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "blendmode.h"

e_blend_mode string_to_blend_mode(const char *string) {
    if (strcasecmp(string, "none") == 0)
        return blend_mode_none;
    if (strcasecmp(string, "blend") == 0)
        return blend_mode_blend;
    if (strcasecmp(string, "additive") == 0)
        return blend_mode_add;
    if (strcasecmp(string, "modulus") == 0)
        return blend_mode_mod;
    if (strcasecmp(string, "multiply") == 0)
        return blend_mode_mul;

    return blend_mode_invalid;
}

const char *blend_mode_to_string(e_blend_mode blend_mode) {
    switch (blend_mode) {
    case blend_mode_none:
        return "none";
    case blend_mode_blend:
        return "blend";
    case blend_mode_add:
        return "additive";
    case blend_mode_mod:
        return "modulus";
    case blend_mode_mul:
        return "multiply";
    case blend_mode_invalid:
        return NULL;
    }
}

SDL_BlendMode blend_mode_to_sdl2(e_blend_mode blend_mode) {
    switch (blend_mode) {
    case blend_mode_none:
        return SDL_BLENDMODE_NONE;
    case blend_mode_blend:
        return SDL_BLENDMODE_BLEND;
    case blend_mode_add:
        return SDL_BLENDMODE_ADD;
    case blend_mode_mod:
        return SDL_BLENDMODE_MOD;
    case blend_mode_mul:
        return SDL_BLENDMODE_MUL;
    case blend_mode_invalid:
        return SDL_BLENDMODE_INVALID;
    }
}
