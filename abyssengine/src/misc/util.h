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

#ifndef ABYSS_UTIL_H
#define ABYSS_UTIL_H

#include <SDL2/SDL.h>

void *util_base64_decode(const char *source, unsigned int bytes);
SDL_Texture *util_load_texture_png(const void *source, int *width, int *height);
char *str_replace(char *string, const char *substr, const char *replacement);
char *trim_string(char *str);

#endif // ABYSS_UTIL_H
