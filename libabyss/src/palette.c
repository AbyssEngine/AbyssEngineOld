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

#include "libabyss/log.h"
#include "streamreader.h"
#include <libabyss/palette.h>
#include <stdlib.h>

#define NUM_PALETTE_COLORS 256
#define NUM_TEXT_COLORS 13
#define LIGHT_LEVEL_VARIATIONS 32
#define INV_COLOR_VARIATIONS 16
#define ALPHA_BLEND_COARSE 3
#define ALPHA_BLEND_FINE 256
#define ADDITIVE_BLENDS 256
#define MULTIPLY_BLENDS 256
#define HUE_VARIATIONS 111
#define UNKNOWN_VARIATIONS 14
#define COMPONENT_BLENDS 256
#define TEXT_SHIFTS 13

#define LOAD_COLORS(field, colors, num_colors)                                                                                                       \
    result->field = malloc(sizeof(palette_color) * num_colors);                                                                                      \
    decode_colors(reader, result->field, colors, num_colors);

#define LOAD_TRANSFORM_SINGLE(field)                                                                                                                 \
    result->field = malloc(NUM_PALETTE_COLORS);                                                                                                      \
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

void decode_colors(streamreader *reader, palette_color *dest, int color_bytes, int num_colors) {
    for (int idx = 0; idx < num_colors; idx++) {
        palette_color *color = &dest[idx];
        switch (color_bytes) {
        case 1:
            color->red = color->green = color->blue = streamreader_read_byte(reader);
            color->alpha = 0xFF;
            continue;
        case 3:
            color->red = streamreader_read_byte(reader);
            color->green = streamreader_read_byte(reader);
            color->blue = streamreader_read_byte(reader);
            color->alpha = 0xFF;
            continue;
        case 4:
            color->red = streamreader_read_byte(reader);
            color->green = streamreader_read_byte(reader);
            color->blue = streamreader_read_byte(reader);
            color->alpha = 0xFF;
            streamreader_skip_bytes(reader, 1); // TODO: Not sure if this is ok...
            // color->alpha = streamreader_read_byte(reader);
            continue;
        default:
            log_fatal("invalid color bytes specified: %i", color_bytes);
            exit(EXIT_FAILURE);
        }
    }

    dest[0].alpha = 0x00;
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

    LOAD_COLORS(base_palette, 4, NUM_PALETTE_COLORS)
    LOAD_TRANSFORM_MULTI(light_level_variations, LIGHT_LEVEL_VARIATIONS)
    LOAD_TRANSFORM_MULTI(inv_color_variations, INV_COLOR_VARIATIONS)
    LOAD_TRANSFORM_SINGLE(selected_unit_shift)
    result->alpha_blend = malloc(sizeof(void *) * ALPHA_BLEND_COARSE);
    for (int blend_idx = 0; blend_idx < ALPHA_BLEND_COARSE; blend_idx++) {
        LOAD_TRANSFORM_MULTI(alpha_blend[blend_idx], ALPHA_BLEND_FINE)
    }
    LOAD_TRANSFORM_MULTI(additive_blend, ADDITIVE_BLENDS)
    LOAD_TRANSFORM_MULTI(multiplicative_blend, MULTIPLY_BLENDS)
    LOAD_TRANSFORM_MULTI(hue_variations, HUE_VARIATIONS)
    LOAD_TRANSFORM_SINGLE(red_tones)
    LOAD_TRANSFORM_SINGLE(green_tones)
    LOAD_TRANSFORM_SINGLE(blue_tones)
    LOAD_TRANSFORM_MULTI(unknown_variations, UNKNOWN_VARIATIONS)
    LOAD_TRANSFORM_MULTI(max_component_blend, COMPONENT_BLENDS)
    LOAD_TRANSFORM_SINGLE(darkened_color_shift)
    LOAD_COLORS(text_colors, 3, TEXT_SHIFTS)
    LOAD_TRANSFORM_MULTI(text_color_shifts, TEXT_SHIFTS)

    streamreader_destroy(reader);
    return result;
}

void palette_destroy(palette *source) {
    FREE_TRANSFORM_MULTI(text_color_shifts, TEXT_SHIFTS)
    free(source->text_colors);
    free(source->darkened_color_shift);
    FREE_TRANSFORM_MULTI(max_component_blend, COMPONENT_BLENDS)
    FREE_TRANSFORM_MULTI(unknown_variations, UNKNOWN_VARIATIONS)
    free(source->blue_tones);
    free(source->green_tones);
    free(source->red_tones);
    FREE_TRANSFORM_MULTI(hue_variations, HUE_VARIATIONS)
    FREE_TRANSFORM_MULTI(multiplicative_blend, MULTIPLY_BLENDS)
    FREE_TRANSFORM_MULTI(additive_blend, ADDITIVE_BLENDS)
    for (int blend_idx = 0; blend_idx < ALPHA_BLEND_COARSE; blend_idx++) {
        FREE_TRANSFORM_MULTI(alpha_blend[blend_idx], ALPHA_BLEND_FINE)
    }
    free(source->alpha_blend);
    free(source->selected_unit_shift);
    FREE_TRANSFORM_MULTI(inv_color_variations, INV_COLOR_VARIATIONS)
    FREE_TRANSFORM_MULTI(light_level_variations, LIGHT_LEVEL_VARIATIONS)
    free(source->base_palette);
    free(source);
}
