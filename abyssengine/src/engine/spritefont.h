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

#ifndef ABYSS_SPRITEFONT_H
#define ABYSS_SPRITEFONT_H

#include "../common/blendmode.h"
#include "../common/color.h"

typedef struct spritefont spritefont;

spritefont *spritefont_load(const char *file_path, const char *palette_name);
void spritefont_destroy(spritefont *source);
void spritefont_draw_text(spritefont *source, int x, int y, const char *text, e_blend_mode blend, rgb color_mod);
void spritefont_get_metrics(const spritefont *source, const char *text, int *width, int *height);

#endif // ABYSS_SPRITEFONT_H
