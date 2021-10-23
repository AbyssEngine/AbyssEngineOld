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

#ifndef ABYSS_COMMONDEF_H
#define ABYSS_COMMONDEF_H

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t e_mouse_button;
enum e_mouse_button { mouse_button_left = 0x01, mouse_button_right = 0x02, mouse_button_middle = 0x04 };

typedef uint8_t e_mouse_event_type;
enum e_mouse_event_type {
    mouse_event_type_move,
    mouse_event_type_button,
};

typedef struct mouse_move_event {
    int x;
    int y;
} mouse_move_event;

typedef struct mouse_button_event {
    enum e_mouse_button button;
    bool pressed;
} mouse_button_event;

typedef struct mouse_event_info {
    union {
        mouse_move_event move_event;
        mouse_button_event button_event;
    };
} mouse_event_info;

#endif // ABYSS_COMMONDEF_H
