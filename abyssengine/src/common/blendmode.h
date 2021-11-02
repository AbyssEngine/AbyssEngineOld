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

#ifndef ABYSS_BLENDMODE_H
#define ABYSS_BLENDMODE_H

#include <SDL.h>

typedef enum { blend_mode_none, blend_mode_blend, blend_mode_add, blend_mode_mod, blend_mode_mul, blend_mode_invalid } e_blend_mode;

e_blend_mode string_to_blend_mode(const char *string);
const char *blend_mode_to_string(e_blend_mode blend_mode);
SDL_BlendMode blend_mode_to_sdl2(e_blend_mode blend_mode);

#endif // ABYSS_BLENDMODE_H
