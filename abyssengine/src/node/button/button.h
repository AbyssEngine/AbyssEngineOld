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

#ifndef ABYSS_BUTTON_H
#define ABYSS_BUTTON_H

#include <stdbool.h>
#include "../../engine/spritefont.h"

typedef struct button button;
typedef enum { button_state_normal, button_state_pressed, button_state_hover, button_state_disabled,  button_state_unknown } e_button_state;

button *button_load(spritefont *sprite_font, const char *file_path, const char *palette_name);
void button_destroy(button *source);

const char *button_state_to_string(e_button_state state);
e_button_state string_to_button_state(const char *string);

void button_set_state(button *source, e_button_state state);
e_button_state button_get_state(const button *source);
void button_set_caption(button *source, const char *caption);
const char *button_get_caption(const button *button);
void button_set_x_segments(button *source, uint8_t segments);
uint8_t button_get_x_segments(const button *source);
void button_set_y_segments(button *source, uint8_t segments);
uint8_t button_get_y_segments(const button *source);
void button_set_text_offset_x(button *source, int offset);
int button_get_text_offset_x(const button *source);
void button_set_text_offset_y(button *source, int offset);
int button_get_text_offset_y(const button *source);
void button_set_fixed_width(button *source, int width);
int button_get_fixed_width(const button *source);
void button_set_fixed_height(button *source, int width);
int button_get_fixed_height(const button *source);
void button_set_label_color(button *source, rgb color);
rgb button_get_label_color(const button *source);
void button_set_color_mod(button *source, rgb color);
rgb button_get_color_mod(const button *source);
void button_set_frame_index_normal(button *source, int index);
int button_get_frame_index_normal(const button *source);
void button_set_frame_index_pressed(button *source, int index);
int button_get_frame_index_pressed(const button *source);
void button_set_frame_index_hover(button *source, int index);
int button_get_frame_index_hover(const button *source);
void button_set_frame_index_disabled(button *source, int index);
int button_get_frame_index_disabled(const button *source);
void button_set_frame_index_checked_normal(button *source, int index);
int button_get_frame_index_checked_normal(const button *source);
void button_set_frame_index_checked_pressed(button *source, int index);
int button_get_frame_index_checked_pressed(const button *source);
void button_set_frame_index_checked_hover(button *source, int index);
int button_get_frame_index_checked_hover(const button *source);
void button_set_checked(button *source, bool checked);
bool button_get_checked(const button *source);
void button_set_is_checkbox(button *source, bool is_checkbox);
bool button_get_is_checkbox(const button *source);
void button_set_lua_activate_callback(button *source, int lua_function_ref);
int button_get_lua_activate_callback(const button *source);
#endif // ABYSS_BUTTON_H
