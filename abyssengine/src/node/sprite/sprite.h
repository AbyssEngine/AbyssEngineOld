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

#ifndef ABYSS_SPRITE_H
#define ABYSS_SPRITE_H

#include "../../engine/engine.h"
#include "../node.h"

typedef uint8_t e_sprite_blend_mode;

enum e_sprite_blend_mode {
    sprite_blend_mode_none,
    sprite_blend_mode_blend,
    sprite_blend_mode_add,
    sprite_blend_mode_mod,
    sprite_blend_mode_mul,
    sprite_blend_mode_unknown
};

const char *blend_mode_to_string(enum e_sprite_blend_mode blend_mode);
enum e_sprite_blend_mode string_to_blend_mode(const char *string);

typedef int e_sprite_play_mode;

enum e_sprite_play_mode { sprite_play_mode_paused, sprite_play_mode_forwards, sprite_play_mode_backwards, sprite_play_mode_unknown };
const char *play_mode_to_string(enum e_sprite_play_mode play_mode);
enum e_sprite_play_mode string_to_play_mode(const char *string);

typedef struct sprite sprite;

sprite *sprite_load(const char *file_path, const char *palette_name);
void sprite_destroy(sprite *source);
void sprite_regenerate_atlas(sprite *source);
uint32_t sprite_get_animation(sprite *source);
void sprite_set_animation(sprite *source, uint32_t animation_idx);
uint32_t sprite_get_frame(sprite *source);
void sprite_set_frame(sprite *source, uint32_t frame_idx);
void sprite_set_cell_size(sprite *source, int cell_size_x, int cell_size_y);
void sprite_get_cell_size(sprite *source, int *cell_size_x, int *cell_size_y);
void sprite_set_bottom_origin(sprite *source, bool is_bottom_origin);
bool sprite_get_bottom_origin(const sprite *source);
void sprite_set_blend_mode(sprite *source, enum e_sprite_blend_mode blend_mode);
e_sprite_blend_mode sprite_get_blend_mode(const sprite *source);
void sprite_set_play_mode(sprite *source, enum e_sprite_play_mode play_mode);
e_sprite_play_mode sprite_get_play_mode(const sprite *source);
void sprite_set_play_length(sprite *source, float play_length);
float sprite_get_play_length(sprite *source);
uint32_t sprite_get_animation_count(const sprite *source);
uint32_t sprite_get_frames_per_animation(const sprite *source);

void sprite_set_lua_mouse_enter_callback(sprite *source, int lua_function_ref);
int sprite_get_lua_mouse_enter_callback(const sprite *source);
void sprite_set_lua_mouse_leave_callback(sprite *source, int lua_function_ref);
int sprite_get_lua_mouse_leave_callback(const sprite *source);
void sprite_set_lua_mouse_up_callback(sprite *source, int lua_function_ref);
int sprite_get_lua_mouse_up_callback(const sprite *source);
void sprite_set_lua_mouse_down_callback(sprite *source, int lua_function_ref);
int sprite_get_lua_mouse_down_callback(const sprite *source);
void sprite_set_lua_mouse_move_callback(sprite *source, int lua_function_ref);
int sprite_get_lua_mouse_move_callback(const sprite *source);

void sprite_render_callback(node *node, engine *source, int offset_x, int offset_y);
void sprite_remove_callback(node *node, engine *source);
void sprite_destroy_callback(node *node, engine *source);
bool sprite_update_callback(node *node, engine *source, uint32_t ticks);
bool sprite_mouse_event_callback(node *node, struct engine *e, enum e_mouse_event_type event_type, const mouse_event_info *event_info);

#endif // ABYSS_SPRITE_H
