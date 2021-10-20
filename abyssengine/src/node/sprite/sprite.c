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

#include "sprite.h"
#include "../../engine/engine.h"
#include "../../scripting/scripting.h"
#include "../node.h"
#include "libabyss/dc6.h"
#include "libabyss/dcc.h"
#include "libabyss/log.h"
#include "libabyss/threading.h"
#include "libabyss/utils.h"
#include <stdlib.h>

typedef uint8_t e_sprite_type;

enum e_sprite_type { sprite_type_dc6, sprite_type_dcc };

typedef struct sprite_frame_pos {
    SDL_Rect rect;
    int offset_x;
    int offset_y;
} sprite_frame_pos;

typedef struct sprite {
    node node;
    e_sprite_type sprite_type;
    SDL_Texture *atlas;
    sprite_frame_pos *frame_rects;
    const palette *palette;
    union {
        dcc *dcc_data;
        dc6 *dc6_data;
    } image_data;
    uint32_t current_frame;
    uint32_t total_frames;
    uint32_t current_direction;
    uint32_t total_directions;
} sprite;

void sprite_regenerate_atlas_dispatch(void *data) { sprite_regenerate_atlas((sprite *)data); }

sprite *sprite_load(const char *file_path, const char *palette_name) {
    size_t path_len = strlen(file_path) - 4;
    if (path_len < 5) {
        return NULL;
    }

    engine *engine = engine_get_global_instance();

    const palette *palette = engine_get_palette(engine, palette_name);

    if (palette == NULL) {
        return NULL;
    }

    sprite *result = calloc(1, sizeof(sprite));
    result->palette = palette;

    const char *path_end = &file_path[path_len];
    char *path_ext = strdup(path_end);
    for (int i = 1; i < 4; i++)
        path_ext[i] = (char)tolower(path_ext[i]);
    if (strcmp(path_ext, ".dcc") == 0) {
        log_fatal("DCC sprites not yet supported!");
        exit(-1);
        //        result->sprite_type = sprite_type_dcc;
        //        char *path_tmp = strdup(file_path);
        //        char *path_new = util_fix_mpq_path(path_tmp);
        //
        //        int size;
        //        char *data = loader_load(engine_get_loader(engine_get_global_instance()), path_new, &size);
        //        free(path_tmp);
        //
        //        if (data == NULL) {
        //            free(path_ext);
        //            free(result);
        //            return NULL;
        //        }
        //
        //        dcc *res = dcc_new_from_bytes(data, size);
        //
        //        free(path_ext);
        //        return result;
    } else if (strcmp(path_ext, ".dc6") == 0) {
        result->sprite_type = sprite_type_dc6;
        char *path_tmp = strdup(file_path);
        char *path_new = util_fix_mpq_path(path_tmp);

        int size;
        char *data = loader_load(engine_get_loader(engine_get_global_instance()), path_new, &size);
        free(path_tmp);

        if (data == NULL) {
            free(path_ext);
            free(result);
            return NULL;
        }

        result->image_data.dc6_data = dc6_new_from_bytes(data, size);
        result->total_frames = result->image_data.dc6_data->frames_per_direction;
        result->total_directions = result->image_data.dc6_data->number_of_directions;

        free(path_ext);
    } else {
        free(result);
        free(path_ext);
        return NULL;
    }

    result->node.active = true;
    result->node.visible = true;
    result->node.render_callback = sprite_render_callback;
    result->node.update_callback = sprite_update_callback;
    result->node.remove_callback = sprite_remove_callback;
    result->node.destroy_callback = sprite_destroy_callback;

    engine_dispatch(engine_get_global_instance(), sprite_regenerate_atlas_dispatch, result);
    return result;
}
void sprite_regenerate_atlas_dc6(sprite *source) {
    VERIFY_ENGINE_THREAD

    if (source->atlas != NULL) {
        SDL_DestroyTexture(source->atlas);
    }

    dc6 *dc6 = source->image_data.dc6_data;

    int atlas_width = 0;
    int atlas_height = 0;

    for (int dir_idx = 0; dir_idx < dc6->number_of_directions; dir_idx++) {
        dc6_direction *direction = &dc6->directions[dir_idx];
        int direction_max_width = 0;
        int direction_max_height = 0;
        for (int frame_idx = 0; frame_idx < dc6->frames_per_direction; frame_idx++) {
            dc6_frame *frame = &direction->frames[frame_idx];

            direction_max_width += (int)frame->width;
            direction_max_height = (direction_max_height < frame->height) ? (int)frame->height : (int)direction_max_height;
        }

        atlas_width = (atlas_width < direction_max_width) ? direction_max_width : atlas_width;
        atlas_height += direction_max_height;
    }

    if (source->atlas != NULL) {
        SDL_DestroyTexture(source->atlas);
    }
    source->atlas = SDL_CreateTexture(engine_get_renderer(engine_get_global_instance()), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC,
                                      atlas_width, atlas_height);

    if (source->frame_rects != NULL) {
        free(source->frame_rects);
    }
    source->frame_rects =
        malloc(sizeof(sprite_frame_pos) * source->image_data.dc6_data->frames_per_direction * source->image_data.dc6_data->number_of_directions);
    uint32_t *buffer = malloc(sizeof(uint32_t) * atlas_width * atlas_height);

    int start_x = 0;
    int start_y = 0;

    for (int dir_idx = 0; dir_idx < dc6->number_of_directions; dir_idx++) {
        dc6_direction *direction = &dc6->directions[dir_idx];
        int direction_max_height = 0;

        for (int frame_idx = 0; frame_idx < dc6->frames_per_direction; frame_idx++) {
            dc6_frame *frame = &direction->frames[frame_idx];
            sprite_frame_pos *frame_rect = &source->frame_rects[(dir_idx * dc6->frames_per_direction) + frame_idx];
            frame_rect->rect.x = start_x;
            frame_rect->rect.y = start_y;
            frame_rect->rect.w = (int)frame->width;
            frame_rect->rect.h = (int)frame->height;
            frame_rect->offset_x = frame->offset_x;
            frame_rect->offset_y = frame->offset_y;

            for (int y = 0; y < frame->height; y++) {
                for (int x = 0; x < frame->width; x++) {
                    palette_color *color = &source->palette->base_palette[frame->index_data[x + (y * frame->width)]];
                    buffer[(start_x + x) + ((start_y + y) * atlas_width)] = *(uint32_t *)color;
                }
            }

            direction_max_height = (direction_max_height < frame->height) ? (int)frame->height : (int)direction_max_height;
            start_x += (int)frame->width;
        }

        start_x = 0;
        start_y += direction_max_height;
    }

    SDL_UpdateTexture(source->atlas, NULL, buffer, atlas_width * 4);
    SDL_SetTextureBlendMode(source->atlas, SDL_BLENDMODE_BLEND);
    free(buffer);
}

void sprite_regenerate_atlas(sprite *source) {
    VERIFY_ENGINE_THREAD

    switch (source->sprite_type) {
    case sprite_type_dc6:
        sprite_regenerate_atlas_dc6(source);
        return;
    default:
        log_fatal("unsupported sprite type");
        exit(-1);
    }
}

void sprite_render_callback(node *node, engine *e) {
    sprite *source = (sprite *)node;
    SDL_Renderer *renderer = engine_get_renderer(e);

    sprite_frame_pos *pos = &source->frame_rects[(source->current_direction * source->total_frames) + source->current_frame];
    SDL_Rect *source_rect = &pos->rect;
    SDL_Rect dest_rect = *source_rect;
    dest_rect.x = source->node.x + pos->offset_x;
    dest_rect.y = source->node.y + pos->offset_y;
    SDL_RenderCopy(renderer, source->atlas, source_rect, &dest_rect);
    // TODO: Render
}

void sprite_remove_callback(node *node, engine *e) {
    sprite *source = (sprite *)node;
    // TODO: Remove
}

void sprite_destroy_callback(node *node, engine *e) {
    sprite *source = (sprite *)node;
    sprite_destroy(source);
}

void sprite_update_callback(node *node, engine *e, uint32_t ticks) {
    sprite *source = (sprite *)node;
    // TODO: Animations
}

void sprite_destroy(sprite *source) {
    if (source->atlas != NULL) {
        SDL_DestroyTexture(source->atlas);
    }
    switch (source->sprite_type) {
    case sprite_type_dc6:
        dc6_destroy(source->image_data.dc6_data);
        break;
    case sprite_type_dcc:
        dcc_destroy(source->image_data.dcc_data);
        break;
    default:
        log_fatal("undefined sprite type");
        exit(-1);
    }

    free(source->frame_rects);
    free(source);
}
