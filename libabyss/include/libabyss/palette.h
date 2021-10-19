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

#ifndef LIBABYSS_PALETTE_H
#define LIBABYSS_PALETTE_H

#include <stdint.h>

typedef struct palette_color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
} palette_color;

typedef struct palette {
    palette_color *base_palette;

    uint8_t **light_level_variations;
    uint8_t **inv_color_variations;
    uint8_t *selected_unit_shift;
    uint8_t ***alpha_blend;
    uint8_t **additive_blend;
    uint8_t **multiplicative_blend;
    uint8_t **hue_variations;
    uint8_t *red_tones;
    uint8_t *green_tones;
    uint8_t *blue_tones;
    uint8_t **unknown_variations;
    uint8_t **max_component_blend;
    uint8_t *darkened_color_shift;

    palette_color *text_colors;
    uint8_t **text_color_shifts;
} palette;


palette *palette_new_from_bytes(const void *data, uint64_t size);
void palette_destroy(palette *source);

#endif // LIBABYSS_PALETTE_H
