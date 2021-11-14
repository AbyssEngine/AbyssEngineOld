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

#include "spritefont.h"
#include "../loader/loader.h"
#include "engine.h"
#include "libabyss/dc6.h"
#include "libabyss/utils.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SPRITEFONT_ATLAS_WIDTH 1024

typedef struct spritefont_glyph {
    uint16_t frame_index;
    uint8_t width;
    uint8_t height;
} spritefont_glyph;

typedef struct spritefont_frame_pos {
    SDL_Rect rect;
    int offset_x;
    int offset_y;
} spritefont_frame_pos;

typedef struct spritefont {
    node node;
    spritefont_glyph *glyphs;
    SDL_Texture *atlas;
    dc6 *dc6_data;
    const palette *palette;
    spritefont_frame_pos *frame_rects;
} spritefont;

void spritefont_regenerate_atlas(spritefont *source) {
    VERIFY_ENGINE_THREAD

    if (source->atlas != NULL) {
        SDL_DestroyTexture(source->atlas);
    }

    const dc6 *dc6 = source->dc6_data;

    assert(dc6->number_of_directions == 1);

    const dc6_direction *direction = &dc6->directions[0];

    int atlas_width = 0;
    int atlas_height = 0;
    int cur_x = 0;
    int cur_height = 0;

    for (int frame_idx = 0; frame_idx < (int)dc6->frames_per_direction; frame_idx++) {
        const dc6_frame *frame = &direction->frames[frame_idx];

        if (cur_x + (int)frame->width > MAX_SPRITEFONT_ATLAS_WIDTH) {
            atlas_width = MAX_SPRITEFONT_ATLAS_WIDTH;

            atlas_height += cur_height;
            cur_x = 0;
            cur_height = 0;
        }

        cur_x += (int)frame->width;
        cur_height = (cur_height < (int)frame->height) ? (int)frame->height : cur_height;
    }

    atlas_height += cur_height;

    if (source->atlas != NULL) {
        SDL_DestroyTexture(source->atlas);
    }
    source->atlas = SDL_CreateTexture(engine_get_renderer(engine_get_global_instance()), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC,
                                      atlas_width, atlas_height);

    if (source->frame_rects != NULL) {
        free(source->frame_rects);
    }

    source->frame_rects = malloc(sizeof(spritefont_frame_pos) * source->dc6_data->frames_per_direction);
    const int buffer_size = atlas_width * atlas_height;
    uint32_t *buffer = calloc(1, sizeof(uint32_t) * buffer_size);

    int start_x = 0;
    int start_y = 0;
    cur_height = 0;

    for (int frame_idx = 0; frame_idx < (int)dc6->frames_per_direction; frame_idx++) {
        const dc6_frame *frame = &direction->frames[frame_idx];
        if (source->frame_rects == NULL) {
            log_error("frame rects not defined");
            exit(EXIT_FAILURE);
        }

        spritefont_frame_pos *frame_rect = &source->frame_rects[frame_idx];

        if (start_x + (int)frame->width > MAX_SPRITEFONT_ATLAS_WIDTH) {
            start_x = 0;
            start_y += cur_height;
            cur_height = 0;
        }

        frame_rect->rect.x = start_x;
        frame_rect->rect.y = start_y;
        frame_rect->rect.w = (int)frame->width;
        frame_rect->rect.h = (int)frame->height;
        frame_rect->offset_x = frame->offset_x;
        frame_rect->offset_y = frame->offset_y;

        for (int y = 0; y < (int)frame->height; y++) {
            for (int x = 0; x < (int)frame->width; x++) {
                if (frame->index_data[x + (y * frame->width)] == 0)
                    continue;

                const palette_color *color = &source->palette->base_palette[frame->index_data[x + (y * frame->width)]];

                const int buff_idx = start_x + x + ((start_y + y) * atlas_width);
                if (buff_idx >= buffer_size) {
                    log_fatal("buffer index out of range");
                    exit(EXIT_FAILURE);
                }

                buffer[buff_idx] = ((uint32_t)color->red << 16) | ((uint32_t)color->green << 8) | ((uint32_t)color->blue) | ((uint32_t)0xFF << 24);
            }
        }

        start_x += (int)frame->width;
        cur_height = (cur_height < (int)frame->height) ? (int)frame->height : cur_height;
    }

    SDL_UpdateTexture(source->atlas, NULL, buffer, atlas_width * 4);
    SDL_SetTextureBlendMode(source->atlas, SDL_BLENDMODE_ADD);

    free(buffer);
}

spritefont *spritefont_load(const char *file_path, const char *palette_name) {
    const engine *engine = engine_get_global_instance();
    const palette *palette = engine_get_palette(engine, palette_name);

    if (palette == NULL) {
        return NULL;
    }

    spritefont *result = calloc(1, sizeof(spritefont));

    if (result == NULL) {
        log_error("Could not allocate memory for spritefont");
        exit(EXIT_FAILURE);
    }

    char *new_path = calloc(1, 4096);

    if (new_path == NULL) {
        log_fatal("Failed to initialize memory.");
        free(result);
        return NULL;
    }

    strcat(new_path, file_path);
    strcat(new_path, ".dc6");

    const char *path_fixed = util_fix_mpq_path(new_path);

    int file_size;
    char *data = loader_load(engine_get_loader(engine), path_fixed, &file_size);

    if (data == NULL) {
        log_error("Failed to load %s", path_fixed);
        free(new_path);
        free(result);

        return NULL;
    }

    result->dc6_data = dc6_new_from_bytes(data, file_size);

    if (result->dc6_data == NULL) {
        log_error("Error decoding dc6 from %s", path_fixed);
        free(new_path);
        free(result);
        free(data);

        return NULL;
    }

    memset(new_path, 0, 4096);
    strcat(new_path, file_path);
    strcat(new_path, ".tbl");
    path_fixed = util_fix_mpq_path(new_path);

    free(data);
    data = (char *)loader_load(engine_get_loader(engine_get_global_instance()), path_fixed, &file_size);
    char *data_ptr = data;

    for (int i = 0; i < 5; i++) {
        const char *signature = "Woo!\x01";
        if (*(data_ptr++) != signature[i]) {
            log_error("Invalid signature in font table.");
            free(data);
            free(new_path);
            free(result->glyphs);
            free(result);
            return NULL;
        }
    }

    data_ptr += 7; // Skip unknown bytes

    result->glyphs = malloc(sizeof(spritefont_glyph) * 0x10000);
    uint16_t max_code = 0;

    while (data_ptr < (data + file_size)) {
        const uint16_t code = (uint16_t)data_ptr[0] | (uint16_t)((uint16_t)data_ptr[1] << 8);

        data_ptr += 3;

        assert(code <= 0xFFFF);
        result->glyphs[code].width = *data_ptr++;
        result->glyphs[code].height = *data_ptr++;

        data_ptr += 3; // More unknowns, typically 1, 0, 0

        result->glyphs[code].frame_index = (uint16_t)data_ptr[0] | (uint16_t)((uint16_t)data_ptr[1] << 8);
        data_ptr += 6;

        if (max_code < code)
            max_code = code;
    }

    result->palette = palette;
    spritefont_glyph *new_ptr = realloc(result->glyphs, sizeof(spritefont_glyph) * max_code + 1);

    if (new_ptr == NULL) {
        log_fatal("failed to reallocate spritefont glyph");
        exit(EXIT_FAILURE);
    }

    result->glyphs = new_ptr;

    free(data);
    free(new_path);

    return result;
}

void spritefont_destroy(spritefont *source) {
    VERIFY_ENGINE_THREAD

    if (source->glyphs != NULL)
        free(source->glyphs);

    if (source->atlas != NULL)
        SDL_DestroyTexture(source->atlas);

    free(source);
}
void spritefont_draw_text(spritefont *source, int x, int y, const char *text, e_blend_mode blend, rgb color_mod) {
    VERIFY_ENGINE_THREAD

    SDL_Renderer *renderer = engine_get_renderer(engine_get_global_instance());

    if (source->atlas == NULL) {
        spritefont_regenerate_atlas(source);
    }

    SDL_SetTextureBlendMode(source->atlas, blend_mode_to_sdl2(blend));
    SDL_SetTextureColorMod(source->atlas, color_mod.r, color_mod.g, color_mod.b);

    SDL_Rect target_rect = {x, y, 0, 0};
    const int start_x = x;
    int max_height = 0;

    for (const char *ch = text; *ch != '\0'; ch++) {
        const spritefont_glyph *glyph = &source->glyphs[(int) * ch];

        if (*ch == '\n') {
            target_rect.x = start_x;
            target_rect.y += max_height;
            max_height = 0;

            continue;
        }

        if (*ch != ' ') {
            const spritefont_frame_pos *frame = &source->frame_rects[glyph->frame_index];
            target_rect.w = frame->rect.w;
            target_rect.h = frame->rect.h;
            max_height = (max_height < glyph->height) ? glyph->height : max_height;
            SDL_RenderCopy(renderer, source->atlas, &frame->rect, &target_rect);
        }

        target_rect.x += glyph->width;
    }
}

void spritefont_get_metrics(const spritefont *source, const char *text, int *width, int *height) {
    int x = 0;
    int row_height = 0;
    int max_width = 0;
    int max_height = 0;

    for (const char *ch = text; *ch != '\0'; ch++) {
        const spritefont_glyph *glyph = &source->glyphs[(int) * ch];

        if (*ch == '\n') {
            x = 0;
            max_height += row_height;
            row_height = 0;

            continue;
        }

        row_height = (row_height < glyph->height) ? glyph->height : row_height;
        x += glyph->width;
        max_width = (x > max_width) ? x : max_width;
    }

    max_height += row_height;

    if (width != NULL)
        *width = max_width;

    if (height != NULL)
        *height = max_height;
}
