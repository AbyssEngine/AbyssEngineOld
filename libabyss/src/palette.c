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

#include "streamreader.h"
#include <libabyss/palette.h>
#include <stdlib.h>

#define NUM_PALETTE_COLORS 256
#define NUM_TEXT_COLORS 13
#define LIGHT_LEVEL_VARIATIONS 32
#define INV_COLOR_VARIATIONS 16
#define ALPHA_BLEND_COARSE 3

#define LOAD_TRANSFORM_SINGLE(field)                                                                                                                 \
    result->base_palette = malloc(NUM_PALETTE_COLORS);                                                                                               \
    decode_transform_single(reader, result->field);

#define LOAD_TRANSFORM_MULTI(field, variations)                                                                                                      \
    result->field = malloc(sizeof(uint8_t *) * (variations));                                                                                        \
    for (int i = 0; i < (variations); i++) {                                                                                                         \
        result->field[i] = malloc(NUM_PALETTE_COLORS);                                                                                               \
    }                                                                                                                                                \
    decode_transform_multi(reader, result->field, (variations));

#define FREE_TRANSFORM_MULTI(field, variations)                                                                                                      \
    for (int i = 0; i < variations; i++) {                                                                                                           \
        free(source->field[i]);                                                                                                                      \
    }                                                                                                                                                \
    free(source->field);

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

void decode_colors(streamreader *reader, palette_color *dest, int color_bytes) {
    for (int idx = 0; idx < NUM_PALETTE_COLORS; idx++) {
        palette_color *color = &dest[idx];
        color->red = streamreader_read_byte(reader);
        color->green = streamreader_read_byte(reader);
        color->blue = streamreader_read_byte(reader);
        color->alpha = streamreader_read_byte(reader);
    }
}

void decode_transform_single(streamreader *reader, uint8_t *dest) {
    for (int i = 0; i < NUM_PALETTE_COLORS; i++) {
        dest[i] = streamreader_read_byte(reader);
    }
}

void decode_transform_multi(streamreader *reader, uint8_t **dest, int count) {
    for (int idx = 0; idx < count; idx++) {
        decode_transform_single(reader, dest[idx]);
    }
}

palette *palette_new_from_bytes(const void *data, const uint64_t size) {
    streamreader *reader = streamreader_create(data, size);
    palette *result = calloc(1, sizeof(palette));

    result->base_palette = malloc(sizeof(palette_color) * NUM_PALETTE_COLORS);
    decode_colors(reader, result->base_palette, 4);

    // Lighting Transforms
    LOAD_TRANSFORM_MULTI(light_level_variations, LIGHT_LEVEL_VARIATIONS)
    LOAD_TRANSFORM_MULTI(inv_color_variations, INV_COLOR_VARIATIONS)
    LOAD_TRANSFORM_SINGLE(selected_unit_shift)

    // Blend Mode Transforms
    LOAD_TRANSFORM_MULTI(alpha_blend, ALPHA_BLEND_COARSE)
    // Color Variation Transforms
    // Other Transforms
    // Text Colors
    // Text Color Transforms

    streamreader_destroy(reader);
    return result;
}

void palette_destroy(palette *source) {
    free(source->selected_unit_shift);
    FREE_TRANSFORM_MULTI(inv_color_variations, INV_COLOR_VARIATIONS)
    FREE_TRANSFORM_MULTI(light_level_variations, LIGHT_LEVEL_VARIATIONS)
    free(source->light_level_variations);
    free(source->base_palette);
    free(source);
}
