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

#ifndef LIBABYSS_DC6_H
#define LIBABYSS_DC6_H

#include "palette.h"

typedef struct dc6_frame {
    uint32_t flipped;
    uint32_t width;
    uint32_t height;
    int32_t offset_x;
    int32_t offset_y;
    uint32_t unknown;
    uint32_t next_block;
    uint32_t length;
    uint8_t *frame_data;
    uint8_t terminator[3];
    uint8_t *index_data;
} dc6_frame;

typedef struct dc6_direction {
    dc6_frame *frames;
} dc6_direction;

typedef struct dc6 {
    int32_t version;
    uint32_t flags;
    uint32_t encoding;
    uint8_t termination[4];
    uint32_t number_of_directions;
    uint32_t frames_per_direction;
    dc6_direction *directions;
} dc6;

dc6 *dc6_new_from_bytes(const void *data, uint64_t size);
void dc6_destroy(dc6 *source);

#endif // LIBABYSS_DC6_H
