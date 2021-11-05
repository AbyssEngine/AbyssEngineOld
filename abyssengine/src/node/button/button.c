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

#include "button.h"
#include "../../common/string.h"
#include "../sprite/sprite.h"

typedef struct button {
    node node;
    sprite *sprite;
    spritefont *sprite_font;
    e_button_state button_state;
    char *caption;
    bool checked;
    bool is_checkbox;
    uint8_t x_segments;
    uint8_t y_segments;
    int text_offset_x;
    int text_offset_y;
    int fixed_width;
    int fixed_height;
    rgb label_color;
    rgb color_mod;
    int frame_index_normal;
    int frame_index_pressed;
    int frame_index_hover;
    int frame_index_disabled;
    int frame_index_checked_normal;
    int frame_index_checked_hover;
    int frame_index_checked_pressed;
    int cur_caption_width;
    int cur_caption_height;
} button;

void button_render_callback(node *node, engine *e, int offset_x, int offset_y) {
    if (!node->active || !node->visible)
        return;

    button *source = (button *)node;

    if (source->sprite != NULL) {
        int frame_index = -1;
        switch (source->button_state) {
        case button_state_normal:
            frame_index = source->is_checkbox ? source->frame_index_checked_normal : source->frame_index_normal;
            break;
        case button_state_pressed:
            frame_index = source->is_checkbox ? source->frame_index_checked_pressed : source->frame_index_pressed;
            break;
        case button_state_hover:
            frame_index = source->is_checkbox ? source->frame_index_checked_hover : source->frame_index_hover;
            break;
        case button_state_disabled:
            frame_index = source->frame_index_disabled;
            break;
        case button_state_unknown:
            frame_index = -1;
            break;
        }

        if (frame_index < 0) {
            frame_index = source->frame_index_normal;
        }

        sprite_set_cell_size(source->sprite, source->x_segments, source->y_segments);
        sprite_render(source->sprite, e, frame_index, node->x + offset_x, node->y+ offset_y);
    }

    if (source->caption != NULL) {

        spritefont_draw_text(source->sprite_font,
                             node->x + source->text_offset_x + offset_x + (source->fixed_width / 2) - (source->cur_caption_width / 2),
                             node->y + source->text_offset_y + offset_y + (source->fixed_height / 2) - (source->cur_caption_height / 2),
                             source->caption,
                             blend_mode_mul,
                             source->label_color);
    }

    node_default_render_callback(node, e, offset_x, offset_y);
}

bool button_update_callback(node *node, engine *e, uint32_t ticks) { return node_default_update_callback(node, e, ticks); }

button *button_load(spritefont *sprite_font, const char *file_path, const char *palette_name) {
    sprite *sprite = NULL;

    if (file_path != NULL && strlen(file_path) > 0) {
        sprite = sprite_load(file_path, palette_name);

        if (sprite == NULL) {
            log_error("Failed to load sprite.");
            return NULL;
        }
    }

    button *result = calloc(1, sizeof(button));
    result->sprite_font = sprite_font;
    result->sprite = sprite;
    result->color_mod.r = 0;
    result->color_mod.g = 0;
    result->color_mod.g = 0;
    result->label_color.r = 255;
    result->label_color.g = 255;
    result->label_color.b = 255;
    result->frame_index_normal = 0;
    result->frame_index_disabled = -1;
    result->frame_index_hover = -1;
    result->frame_index_checked_hover = -1;
    result->frame_index_checked_normal = -1;
    result->frame_index_checked_pressed = -1;
    result->frame_index_pressed = -1;
    result->node.update_callback = button_update_callback;
    result->node.render_callback = button_render_callback;
    result->node.active = true;
    result->node.visible = true;
    return result;
}

void button_destroy(button *source) {
    if (source->sprite != NULL)
        sprite_destroy(source->sprite);

    if (source->caption != NULL)
        free(source->caption);

    free(source);
}

const char *button_state_to_string(e_button_state state) {
    switch (state) {
    case button_state_normal:
        return "normal";
    case button_state_pressed:
        return "pressed";
    case button_state_hover:
        return "hover";
    case button_state_disabled:
        return "disabled";
    default:
    case button_state_unknown:
        return NULL;
    }
}

e_button_state string_to_button_state(const char *string) {
    if (compare_str_ncase(string, "normal"))
        return button_state_normal;

    if (compare_str_ncase(string, "pressed"))
        return button_state_pressed;

    if (compare_str_ncase(string, "hover"))
        return button_state_hover;

    if (compare_str_ncase(string, "disabled"))
        return button_state_disabled;

    return button_state_unknown;
}

void button_set_caption(button *source, const char *caption) {
    if (source->caption != NULL)
        free(source->caption);

    source->caption = strdup(caption);
    spritefont_get_metrics(source->sprite_font, caption, &source->cur_caption_width, &source->cur_caption_height);
}

void button_set_state(button *source, e_button_state state) { source->button_state = state; }

e_button_state button_get_state(const button *source) { return source->button_state; }

const char *button_get_caption(const button *source) { return source->caption; }

void button_set_x_segments(button *source, uint8_t segments) { source->x_segments = segments; }

uint8_t button_get_x_segments(const button *source) { return source->x_segments; }

void button_set_y_segments(button *source, uint8_t segments) { source->y_segments = segments; }

uint8_t button_get_y_segments(const button *source) { return source->y_segments; }

void button_set_text_offset_x(button *source, int offset) { source->text_offset_x = offset; }

int button_get_text_offset_x(const button *source) { return source->text_offset_x; }

void button_set_text_offset_y(button *source, int offset) { source->text_offset_y = offset; }

int button_get_text_offset_y(const button *source) { return source->text_offset_y; }

void button_set_fixed_width(button *source, int width) { source->fixed_width = width; }

int button_get_fixed_width(const button *source) { return source->fixed_width; }

void button_set_fixed_height(button *source, int width) { source->fixed_height = width; }

int button_get_fixed_height(const button *source) { return source->fixed_height; }

void button_set_label_color(button *source, rgb color) { source->label_color = color; }

rgb button_get_label_color(const button *source) { return source->label_color; }

void button_set_color_mod(button *source, rgb color) { source->color_mod = color; }

rgb button_get_color_mod(const button *source) { return source->color_mod; }

void button_set_frame_index_normal(button *source, int index) { source->frame_index_normal = index; }

int button_get_frame_index_normal(const button *source) { return source->frame_index_normal; }

void button_set_frame_index_pressed(button *source, int index) { source->frame_index_pressed = index; }

int button_get_frame_index_pressed(const button *source) { return source->frame_index_pressed; }

void button_set_frame_index_hover(button *source, int index) { source->frame_index_hover = index; }

int button_get_frame_index_hover(const button *source) { return source->frame_index_hover; }

void button_set_frame_index_disabled(button *source, int index) { source->frame_index_disabled = index; }

int button_get_frame_index_disabled(const button *source) { return source->frame_index_disabled; }

void button_set_frame_index_checked_normal(button *source, int index) { source->frame_index_checked_normal = index; }

int button_get_frame_index_checked_normal(const button *source) { return source->frame_index_checked_normal; }

void button_set_frame_index_checked_pressed(button *source, int index) { source->frame_index_checked_pressed = index; }

int button_get_frame_index_checked_pressed(const button *source) { return source->frame_index_checked_pressed; }

void button_set_frame_index_checked_hover(button *source, int index) { source->frame_index_checked_hover = index; }

int button_get_frame_index_checked_hover(const button *source) { return source->frame_index_checked_hover; }

void button_set_checked(button *source, bool checked) { source->checked = checked; }

bool button_get_checked(const button *source) { return source->checked; }

void button_set_is_checkbox(button *source, bool is_checkbox) { source->is_checkbox = is_checkbox; }

bool button_get_is_checkbox(const button *source) { return source->is_checkbox; }
