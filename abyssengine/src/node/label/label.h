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

#ifndef ABYSS_LABEL_H
#define ABYSS_LABEL_H

#include "../../common/align.h"
#include "../../common/blendmode.h"
#include "../../common/color.h"
#include "../../engine/spritefont.h"

typedef struct label label;

label *label_create(spritefont *font);
void label_destroy(label *label);
const char *label_get_text(const label *label);
void label_set_text(label *label, const char *text);
e_alignment label_get_horizontal_alignment(const label *label);
void label_set_horizontal_alignment(label *label, e_alignment alignment);
e_alignment label_get_vertical_alignment(const label *label);
void label_set_vertical_alignment(label *label, e_alignment alignment);
e_blend_mode label_get_blend_mode(const label *label);
void label_set_blend_mode(label *label, e_blend_mode blend_mode);
rgb label_get_color_mod(const label *label);
void label_set_color_mod(label *label, rgb color_mod);
#endif // ABYSS_LABEL_H
