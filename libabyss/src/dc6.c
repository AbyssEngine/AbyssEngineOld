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

#define END_OF_SCANLINE 0x80
#define MAX_RUN_LENGTH 0x7F
#define END_OF_LINE 1
#define RUN_OF_TRANSPARENT_PIXELS 2
#define RUN_OF_OPAQUE_PIXELS 3

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

void dc6_decode_footer(dc6 *source, streamreader *reader) {
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
            frame->frame_data = calloc(1, frame->length);
            streamreader_get_bytes(reader, frame->frame_data, frame->length);
            READ_BYTES(frame->terminator, 3)
        }
    }
}

int dc6_get_scanline_type(int b) {
    if (b == END_OF_SCANLINE)
        return END_OF_LINE;

    if ((b & END_OF_SCANLINE) > 0)
        return RUN_OF_TRANSPARENT_PIXELS;

    return RUN_OF_OPAQUE_PIXELS;
}

void dc6_decode_frame(dc6_frame *frame) {
    frame->index_data = calloc(1, frame->width * frame->height);
    int x = 0;
    int y = (int)frame->height - 1;
    int offset = 0;

    bool run = true;
    while (run) {
        int b = frame->frame_data[offset++];

        switch (dc6_get_scanline_type(b)) {
        case END_OF_LINE:
            if (y == 0) {
                run = false;
                continue;
            }
            y--;
            x = 0;
            break;
        case RUN_OF_TRANSPARENT_PIXELS:
            x += (b & MAX_RUN_LENGTH);
            break;
        case RUN_OF_OPAQUE_PIXELS:
            for (int i = 0; i < b; i++) {
                frame->index_data[x + (y * frame->width) + i] = frame->frame_data[offset++];
            }
            x += b;
            break;
        }
    }
}

dc6 *dc6_new_from_bytes(const void *data, uint64_t size) {
    dc6 *result = calloc(1, sizeof(dc6));
    streamreader *reader = streamreader_create(data, size);

    dc6_decode_header(result, reader);
    dc6_decode_footer(result, reader);

    for (int dir_idx = 0; dir_idx < result->number_of_directions; dir_idx++) {
        dc6_direction *direction = &result->directions[dir_idx];
        for (int frame_idx = 0; frame_idx < result->frames_per_direction; frame_idx++) {
            dc6_decode_frame(&direction->frames[frame_idx]);
        }
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
                if (frame->index_data != NULL)
                    free(frame->index_data);
                free(frame->frame_data);
            }
            free(direction->frames);
        }
        free(source->directions);
    }
    free(source);
}
