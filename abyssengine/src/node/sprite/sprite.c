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
#include "../../scripting/scripting.h"
#include "libabyss/dc6.h"
#include "libabyss/dcc.h"
#include "libabyss/log.h"
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
    uint32_t current_animation;
    uint32_t total_animations;
    uint32_t cell_size_x;
    uint32_t cell_size_y;
    bool bottom_origin;
    bool mouse_in_sprite;
    e_sprite_blend_mode blend_mode;
    enum e_sprite_play_mode play_mode;
    bool loop_animation;
    float last_frame_time;
    float play_length;
    int lua_mouse_down_callback_func;
    int lua_mouse_up_callback_func;
    int lua_mouse_enter_callback_func;
    int lua_mouse_leave_callback_func;
    int lua_mouse_move_callback_func;
} sprite;

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
    node_initialize(&result->node);

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
        result->total_animations = result->image_data.dc6_data->number_of_directions;

        free(path_ext);
    } else {
        free(result);
        free(path_ext);
        return NULL;
    }

    result->node.active = true;
    result->node.visible = true;
    result->cell_size_x = 1;
    result->cell_size_y = 1;
    result->bottom_origin = false;
    result->blend_mode = sprite_blend_mode_blend;
    result->play_mode = sprite_play_mode_paused;
    result->loop_animation = true;
    result->play_length = 1.f;
    result->node.render_callback = sprite_render_callback;
    result->node.update_callback = sprite_update_callback;
    result->node.remove_callback = sprite_remove_callback;
    result->node.destroy_callback = sprite_destroy_callback;
    result->node.mouse_event_callback = sprite_mouse_event_callback;

    return result;
}

void sprite_advance_frame(sprite *source) {
    if (source->play_mode == sprite_play_mode_paused || source->play_mode == sprite_play_mode_unknown)
        return;

    uint32_t start_index = 0;
    uint32_t end_index = sprite_get_frames_per_animation(source);

    //    if s.hasSubLoop && s.playedCount > 0 {
    //        startIndex = s.subStartingFrame
    //        endIndex = s.subEndingFrame
    //    }

    if (source->play_mode == sprite_play_mode_forwards) {
        source->current_frame++;

        if (source->current_frame < end_index)
            return;

        source->current_frame = (source->loop_animation) ? start_index : end_index - 1;

        return;
    }

    if (source->current_frame > start_index) {
        source->current_frame--;
        return;
    }

    source->current_frame = (source->loop_animation) ? end_index - 1 : start_index;
}

void sprite_animate(sprite *source, float time_elapsed) {
    if (source->play_mode == sprite_play_mode_paused)
        return;

    uint32_t frame_count = sprite_get_frames_per_animation(source);
    float frame_length = source->play_length / (float)frame_count;
    source->last_frame_time += time_elapsed;
    uint32_t frames_advanced = (uint32_t)(source->last_frame_time / frame_length);
    source->last_frame_time -= (float)frames_advanced * frame_length;

    for (int i = 0; i < frames_advanced; i++)
        sprite_advance_frame(source);
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

    sprite_set_blend_mode(source, source->blend_mode);
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

void sprite_frame_offset(sprite *source, uint32_t frame, int *offset_x, int *offset_y) {
    switch (source->sprite_type) {
    case sprite_type_dc6: {
        dc6_frame *item = &source->image_data.dc6_data->directions[source->current_animation].frames[frame];
        if (offset_x != NULL)
            *offset_x = item->offset_x;

        if (offset_y != NULL)
            *offset_y = item->offset_y;
    } break;
    default:
        log_fatal("invalid sprite type");
        exit(-1);
    }
}

void sprite_frame_size(sprite *source, uint32_t *frame_size_x, uint32_t *frame_size_y) {
    switch (source->sprite_type) {
    case sprite_type_dc6: {
        if (frame_size_x != NULL) {
            *frame_size_x = 0;
            for (int i = 0; i < source->cell_size_x; i++)
                *frame_size_x += source->image_data.dc6_data->directions[source->current_animation].frames[source->current_frame + i].width;
        }

        if (frame_size_y != NULL) {
            *frame_size_y = 0;
            for (int i = 0; i < source->cell_size_y; i++)
                *frame_size_y += source->image_data.dc6_data->directions[source->current_animation].frames[source->current_frame + i].height;
        }
    } break;
    default:
        log_fatal("invalid sprite type");
        exit(-1);
    }
}

void sprite_render_callback(node *node, engine *e, int offset_x, int offset_y) {
    sprite *source = (sprite *)node;

    if (!node->visible || !node->active)
        return;

    if (source->atlas == NULL)
        sprite_regenerate_atlas(source);

    uint32_t frame_height;
    uint32_t frame_width;
    sprite_frame_size(source, &frame_width, &frame_height);

    int pos_x = node->x + offset_x;
    int pos_y = node->y + offset_y;

    if (source->bottom_origin)
        pos_y -= (int)frame_height;

    SDL_Renderer *renderer = engine_get_renderer(e);

    int start_x = pos_x;

    for (int cell_offset_y = 0; cell_offset_y < source->cell_size_y; cell_offset_y++) {
        int last_height = 0;

        for (int cell_offset_x = 0; cell_offset_x < source->cell_size_x; cell_offset_x++) {
            uint32_t cell_index = source->current_frame + (cell_offset_x + (cell_offset_y * source->cell_size_x));

            int frame_offset_x;
            int frame_offset_y;
            sprite_frame_offset(source, cell_index, &frame_offset_x, &frame_offset_y);

            sprite_frame_pos *pos = &source->frame_rects[(source->current_animation * source->total_frames) + cell_index];
            SDL_Rect *source_rect = &pos->rect;
            SDL_Rect dest_rect = *source_rect;
            dest_rect.x = pos->offset_x + pos_x;
            dest_rect.y = pos->offset_y + pos_y;
            SDL_RenderCopy(renderer, source->atlas, source_rect, &dest_rect);

            pos_x += source_rect->w;
            last_height = source_rect->h;
        }

        pos_x = start_x;
        pos_y += last_height;
    }

    node_default_render_callback(node, e, offset_x, offset_y);
}

void sprite_remove_callback(node *node, engine *e) {
    sprite *source = (sprite *)node;
    // TODO: Remove
}

void sprite_destroy_callback(node *node, engine *e) {
    sprite *source = (sprite *)node;
    sprite_destroy(source);
}

bool sprite_update_callback(node *node, engine *e, uint32_t ticks) {
    sprite *source = (sprite *)node;

    sprite_animate(source, (float)ticks / 1000.f);

    return node_default_update_callback(node, e, ticks);
}

void sprite_destroy(sprite *source) {
    if (source->atlas != NULL)
        SDL_DestroyTexture(source->atlas);

    if (source->lua_mouse_down_callback_func != 0)
        luaL_unref(engine_get_lua_state(engine_get_global_instance()), LUA_REGISTRYINDEX, source->lua_mouse_down_callback_func);

    if (source->lua_mouse_up_callback_func != 0)
        luaL_unref(engine_get_lua_state(engine_get_global_instance()), LUA_REGISTRYINDEX, source->lua_mouse_up_callback_func);

    if (source->lua_mouse_enter_callback_func != 0)
        luaL_unref(engine_get_lua_state(engine_get_global_instance()), LUA_REGISTRYINDEX, source->lua_mouse_enter_callback_func);

    if (source->lua_mouse_leave_callback_func != 0)
        luaL_unref(engine_get_lua_state(engine_get_global_instance()), LUA_REGISTRYINDEX, source->lua_mouse_leave_callback_func);

    if (source->lua_mouse_move_callback_func != 0)
        luaL_unref(engine_get_lua_state(engine_get_global_instance()), LUA_REGISTRYINDEX, source->lua_mouse_move_callback_func);

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

uint32_t sprite_get_animation(sprite *source) { return source->current_animation; }

void sprite_set_animation(sprite *source, uint32_t animation_idx) {
    source->current_animation = animation_idx;
    source->current_frame = 0;
    if (source->current_animation < source->total_animations)
        return;

    source->current_animation = source->total_animations - 1;
}

uint32_t sprite_get_frame(sprite *source) { return source->current_frame; }

void sprite_set_frame(sprite *source, uint32_t frame_idx) {
    source->current_frame = frame_idx;
    if (source->current_frame < source->total_frames)
        return;

    source->current_frame = source->total_frames - 1;
}

void sprite_set_cell_size(sprite *source, int cell_size_x, int cell_size_y) {
    source->cell_size_x = cell_size_x;
    source->cell_size_y = cell_size_y;
}

void sprite_get_cell_size(sprite *source, int *cell_size_x, int *cell_size_y) {
    if (cell_size_x != NULL)
        *cell_size_x = (int)source->cell_size_x;

    if (cell_size_x != NULL)
        *cell_size_y = (int)source->cell_size_x;
}

void sprite_set_bottom_origin(sprite *source, bool is_bottom_origin) { source->bottom_origin = is_bottom_origin; }

bool sprite_get_bottom_origin(const sprite *source) { return source->bottom_origin; }

void sprite_set_blend_mode(sprite *source, enum e_sprite_blend_mode blend_mode) {
    source->blend_mode = blend_mode;

    SDL_BlendMode new_mode;
    switch (blend_mode) {
    case sprite_blend_mode_none:
        new_mode = SDL_BLENDMODE_NONE;
        break;
    case sprite_blend_mode_blend:
        new_mode = SDL_BLENDMODE_BLEND;
        break;
    case sprite_blend_mode_add:
        new_mode = SDL_BLENDMODE_ADD;
        break;
    case sprite_blend_mode_mod:
        new_mode = SDL_BLENDMODE_MOD;
        break;
    case sprite_blend_mode_mul:
        new_mode = SDL_BLENDMODE_MUL;
        break;
    default:
        log_fatal("invalid blend mode");
        exit(EXIT_FAILURE);
    }


    source->blend_mode = blend_mode;
    if (source->atlas != NULL) 
        SDL_SetTextureBlendMode(source->atlas, new_mode);
}

e_sprite_blend_mode sprite_get_blend_mode(const sprite *source) { return source->blend_mode; }

const char *blend_mode_to_string(enum e_sprite_blend_mode blend_mode) {
    switch (blend_mode) {
    case sprite_blend_mode_none:
        return "none";
    case sprite_blend_mode_blend:
        return "blend";
    case sprite_blend_mode_add:
        return "additive";
    case sprite_blend_mode_mod:
        return "modulate";
    case sprite_blend_mode_mul:
        return "multiply";
    default:
        return NULL;
    }
}

void sprite_set_play_mode(sprite *source, enum e_sprite_play_mode play_mode) {
    source->play_mode = play_mode;
    source->last_frame_time = 0.0f;
}

e_sprite_play_mode sprite_get_play_mode(const sprite *source) { return source->play_mode; }

enum e_sprite_blend_mode string_to_blend_mode(const char *string) {
    char *mode_str = strdup(string);
    for (char *ch = mode_str; *ch != '\0'; ch++)
        *ch = (char)tolower(*ch);

    enum e_sprite_blend_mode blend_mode = sprite_blend_mode_unknown;

    if (strcmp(mode_str, "none") == 0)
        blend_mode = sprite_blend_mode_none;

    else if (strcmp(mode_str, "blend") == 0)
        blend_mode = sprite_blend_mode_blend;

    else if (strcmp(mode_str, "additive") == 0)
        blend_mode = sprite_blend_mode_add;

    else if (strcmp(mode_str, "modulate") == 0)
        blend_mode = sprite_blend_mode_mod;

    else if (strcmp(mode_str, "multiply") == 0)
        blend_mode = sprite_blend_mode_mul;

    free(mode_str);

    return blend_mode;
}

const char *play_mode_to_string(enum e_sprite_play_mode play_mode) {
    switch (play_mode) {
    case sprite_play_mode_paused:
        return "paused";
    case sprite_play_mode_forwards:
        return "forwards";
    case sprite_play_mode_backwards:
        return "backwards";
    default:
        return NULL;
    }
}

enum e_sprite_play_mode string_to_play_mode(const char *string) {
    char *mode_str = strdup(string);
    for (char *ch = mode_str; *ch != '\0'; ch++)
        *ch = (char)tolower(*ch);

    enum e_sprite_play_mode play_mode = sprite_play_mode_unknown;
    if (strcmp(mode_str, "paused") == 0)
        play_mode = sprite_play_mode_paused;

    if (strcmp(mode_str, "forwards") == 0)
        play_mode = sprite_play_mode_forwards;

    if (strcmp(mode_str, "backwards") == 0)
        play_mode = sprite_play_mode_backwards;

    free(mode_str);

    return play_mode;
}

void sprite_set_play_length(sprite *source, float play_length) { source->play_length = play_length; }

float sprite_get_play_length(sprite *source) { return source->play_length; }

uint32_t sprite_get_animation_count(const sprite *source) {
    switch (source->sprite_type) {
    case sprite_type_dc6:
        return source->image_data.dc6_data->number_of_directions;
    default:
        log_fatal("unsupported animation type");
        exit(EXIT_FAILURE);
    }
}

uint32_t sprite_get_frames_per_animation(const sprite *source) {
    switch (source->sprite_type) {
    case sprite_type_dc6:
        return source->image_data.dc6_data->frames_per_direction;
    default:
        log_fatal("unsupported animation type");
        exit(EXIT_FAILURE);
    }
}

void sprite_set_lua_mouse_move_callback(sprite *source, int lua_function_ref) {
    if (source->lua_mouse_move_callback_func != 0)
        luaL_unref(engine_get_lua_state(engine_get_global_instance()), LUA_REGISTRYINDEX, source->lua_mouse_move_callback_func);

    source->lua_mouse_move_callback_func = lua_function_ref;
}

int sprite_get_lua_mouse_move_callback(const sprite *source) { return source->lua_mouse_move_callback_func; }

void sprite_set_lua_mouse_enter_callback(sprite *source, int lua_function_ref) {
    if (source->lua_mouse_enter_callback_func != 0)
        luaL_unref(engine_get_lua_state(engine_get_global_instance()), LUA_REGISTRYINDEX, source->lua_mouse_enter_callback_func);

    source->lua_mouse_enter_callback_func = lua_function_ref;
}

int sprite_get_lua_mouse_enter_callback(const sprite *source) { return source->lua_mouse_enter_callback_func; }

void sprite_set_lua_mouse_leave_callback(sprite *source, int lua_function_ref) {
    if (source->lua_mouse_leave_callback_func != 0)
        luaL_unref(engine_get_lua_state(engine_get_global_instance()), LUA_REGISTRYINDEX, source->lua_mouse_leave_callback_func);

    source->lua_mouse_leave_callback_func = lua_function_ref;
}

int sprite_get_lua_mouse_leave_callback(const sprite *source) { return source->lua_mouse_leave_callback_func; }

void sprite_set_lua_mouse_up_callback(sprite *source, int lua_function_ref) {
    if (source->lua_mouse_up_callback_func != 0)
        luaL_unref(engine_get_lua_state(engine_get_global_instance()), LUA_REGISTRYINDEX, source->lua_mouse_up_callback_func);

    source->lua_mouse_up_callback_func = lua_function_ref;
}

int sprite_get_lua_mouse_up_callback(const sprite *source) { return source->lua_mouse_up_callback_func; }

void sprite_set_lua_mouse_down_callback(sprite *source, int lua_function_ref) {
    if (source->lua_mouse_down_callback_func != 0)
        luaL_unref(engine_get_lua_state(engine_get_global_instance()), LUA_REGISTRYINDEX, source->lua_mouse_down_callback_func);

    source->lua_mouse_down_callback_func = lua_function_ref;
}

int sprite_get_lua_mouse_down_callback(const sprite *source) { return source->lua_mouse_down_callback_func; }

bool sprite_mouse_event_callback(node *node, struct engine *e, enum e_mouse_event_type event_type, const mouse_event_info *event_info) {
    sprite *source = (sprite *)node;
    lua_State *l = engine_get_lua_state(e);
    bool handle_input_event = false;

    switch (event_type) {
    case mouse_event_type_button: {
        if (!source->mouse_in_sprite)
            break;

        int lua_func = (event_info->button_event.pressed) ? source->lua_mouse_down_callback_func : source->lua_mouse_up_callback_func;
        if (lua_func != 0) {
            lua_rawgeti(l, LUA_REGISTRYINDEX, lua_func);
            lua_pushnumber(l, event_info->button_event.button);
            lua_call(l, 1, 1);
            handle_input_event = lua_toboolean(l, -1);
        }

    } break;
    case mouse_event_type_move: {
        int mx = event_info->move_event.x;
        int my = event_info->move_event.y;
        
        int sx = node->x;
        int sy = node->y;
        int sx2 = sx;
        int sy2 = sy;
        uint32_t fx;
        uint32_t fy;
        sprite_frame_size(source, &fx, &fy);
        sx2 += (int)fx;
        sy2 += (int)fy;

        if ((mx < sx) || (mx >= sx2) || (my < sy) || (my >= sy2)) {
            if (source->mouse_in_sprite) {
                source->mouse_in_sprite = false;

                if (source->lua_mouse_leave_callback_func != 0) {
                    lua_rawgeti(l, LUA_REGISTRYINDEX, source->lua_mouse_leave_callback_func);
                    lua_pushnumber(l, event_info->move_event.x);
                    lua_pushnumber(l, event_info->move_event.y);
                    lua_call(l, 2, 1);
                    handle_input_event = lua_toboolean(l, -1);
                }
            }
            break;
        }

        if (!source->mouse_in_sprite) {
            source->mouse_in_sprite = true;

            if (source->lua_mouse_enter_callback_func != 0) {
                lua_rawgeti(l, LUA_REGISTRYINDEX, source->lua_mouse_enter_callback_func);
                lua_pushnumber(l, event_info->move_event.x);
                lua_pushnumber(l, event_info->move_event.y);
                lua_call(l, 2, 1);
                handle_input_event = lua_toboolean(l, -1);
            }
            if (handle_input_event)
                break;
        }

        if (source->lua_mouse_move_callback_func != 0) {
            lua_rawgeti(l, LUA_REGISTRYINDEX, source->lua_mouse_move_callback_func);
            lua_pushnumber(l, event_info->move_event.x);
            lua_pushnumber(l, event_info->move_event.y);
            lua_call(l, 2, 1);
            handle_input_event = lua_toboolean(l, -1);
        }

    } break;
    default:
        break;
    }

    if (handle_input_event)
        return true;

    return node_default_mouse_event_callback(node, e, event_type, event_info);
}
