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

typedef struct sprite {
    node node;
    e_sprite_type sprite_type;
    SDL_Texture *atlas;
    const palette *palette;
    union {
        dcc *dcc_data;
        dc6 *dc6_data;
    } image_data;
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

            direction_max_width += frame->width;
            direction_max_height = (direction_max_height < frame->height) ? frame->height : direction_max_height;
        }

        atlas_width = (atlas_width < direction_max_width) ? direction_max_width : atlas_width;
        atlas_height += direction_max_height;
    }

    source->atlas = SDL_CreateTexture(engine_get_renderer(engine_get_global_instance()), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC,
                                      atlas_width, atlas_height);

    uint8_t *buffer = calloc(1, 4 * atlas_width * atlas_height);

    int start_x = 0;
    int start_y = 0;

    for (int dir_idx = 0; dir_idx < dc6->number_of_directions; dir_idx++) {
        dc6_direction *direction = &dc6->directions[dir_idx];
        int direction_max_height = 0;

        for (int frame_idx = 0; frame_idx < dc6->frames_per_direction; frame_idx++) {
            dc6_frame *frame = &direction->frames[frame_idx];

            for (int y = 0; y < frame->height; y++) {
                for (int x = 0; x < frame->width; x++) {
//                    palette_color *color = &source->palette->base_palette[frame->index_data[palette_idx]];
//                    int buffer_idx = ((start_x + x) + ((start_y + y) * atlas_width)) * 4;
//                    if (buffer_idx >= (atlas_width * atlas_height * 4)) {
//                        break;
//                    }
                    //                    buffer[buffer_idx] = 0x00;
                    //                    buffer[buffer_idx + 1] = 0x00;
                    //                    buffer[buffer_idx + 2] = 0x00;
                    //                    buffer[buffer_idx + 3] = 0xFF;
                    //                    buffer[buffer_idx] =
                    //                        ((uint32_t)color->blue << 24) | ((uint32_t)color->green << 16) | ((uint32_t)color->red << 8) |
                    //                        ((uint32_t)color->alpha);
                }
            }

            direction_max_height = (direction_max_height < frame->height) ? frame->height : direction_max_height;
            start_x += (int)frame->width;
        }

        start_x = 0;
        start_y += direction_max_height;
    }

    SDL_UpdateTexture(source->atlas, NULL, buffer, atlas_width * 4);
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

    SDL_RenderCopy(renderer, source->atlas, NULL, NULL);
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
    free(source);
}
