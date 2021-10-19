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

#include "streamreader.h"
#include <libabyss/dc6.h>
#include <stdint.h>
#include <stdlib.h>

#define READ_BYTE(field) field = streamreader_read_byte(reader);
#define READ_UINT32(field) field = streamreader_read_uint32(reader);
#define READ_INT32(field) field = streamreader_read_int32(reader);
#define READ_BYTES(field, bytes)                                                                                                                     \
    for (int __x = 0; __x < bytes; __x++) {                                                                                                          \
        field[__x] = streamreader_read_byte(reader);                                                                                                 \
    }

void dc6_decode_header(dc6 *source, streamreader *reader) {
    READ_INT32(source->version)
    READ_UINT32(source->flags)
    READ_UINT32(source->encoding)
    READ_BYTES(source->termination, 4)
}

bool dc6_decode_footer(dc6 *source, streamreader *reader) {
    READ_UINT32(source->number_of_directions)
    READ_UINT32(source->frames_per_direction)

    source->directions = calloc(source->number_of_directions, sizeof(dc6_direction));

    for (int idx = 0; idx < source->number_of_directions; idx++) {
        source->directions[idx].frames = calloc(source->frames_per_direction, sizeof(dc6_frame));
    }

    uint32_t total_frames = source->number_of_directions * source->frames_per_direction;
    streamreader_skip_bytes(reader, 4 * total_frames); // Skip the frame pointers

    for (uint32_t dir_idx = 0; dir_idx < source->number_of_directions; dir_idx++) {
        for (uint32_t frame_idx = 0; frame_idx < source->frames_per_direction; frame_idx++) {
            dc6_frame *frame = &source->directions[dir_idx].frames[frame_idx];

            READ_UINT32(frame->flipped)
            READ_UINT32(frame->width)
            READ_UINT32(frame->height)
            READ_INT32(frame->offset_x)
            READ_INT32(frame->offset_y)
            READ_UINT32(frame->unknown)
            READ_UINT32(frame->next_block)
            READ_UINT32(frame->length)
            frame->frame_data = calloc(1, sizeof(frame->length));
            streamreader_get_bytes(reader, frame->frame_data, frame->length);
            READ_BYTES(frame->terminator, 3)
        }
    }
    return true;
}

dc6 *dc6_new_from_bytes(const void *data, uint64_t size) {
    dc6 *result = calloc(1, sizeof(dc6));
    streamreader *reader = streamreader_create(data, size);

    dc6_decode_header(result, reader);

    if (!dc6_decode_footer(result, reader)) {
        free(result);
        streamreader_destroy(reader);
        return NULL;
    }

    streamreader_destroy(reader);
    return result;
}

void dc6_destroy(dc6 *source) {
    if (source->directions != NULL) {
        for (int dir_idx = 0; dir_idx < source->number_of_directions; dir_idx++) {
            dc6_direction *direction = &source->directions[dir_idx];
            for (int frame_idx = 0; frame_idx < source->frames_per_direction; frame_idx++) {
                dc6_frame *frame = &direction->frames[frame_idx];
                free(frame->frame_data);
            }
            free(direction->frames);
        }
        free(source->directions);
    }
    free(source);
}
