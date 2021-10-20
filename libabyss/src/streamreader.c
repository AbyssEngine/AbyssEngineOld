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
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct streamreader {
    const char *data;
    uint64_t data_size;
    uint64_t position;
} streamreader;

streamreader *streamreader_create(const void *data, const uint64_t data_size) {
    streamreader *result = malloc(sizeof(streamreader));

    assert(data_size > 0);

    result->data = (char *)data;
    result->position = 0;
    result->data_size = data_size;

    return result;
}

void streamreader_destroy(streamreader *source) { free(source); }

uint8_t streamreader_read_byte(streamreader *source) {
    assert(source->position < source->data_size);
    if (source->position >= source->data_size) {
        exit(-1);
    }
    return source->data[source->position++];
}

int16_t streamreader_read_int16(streamreader *source) { return (int16_t)streamreader_read_uint16(source); }

uint16_t streamreader_read_uint16(streamreader *source) {
    uint8_t value[2];
    streamreader_get_bytes(source, value, 2);
    return ((uint16_t)value[0]) | (((uint16_t)value[1]) << 8);
}

int32_t streamreader_read_int32(streamreader *source) { return (int32_t)streamreader_read_uint32(source); }

uint32_t streamreader_read_uint32(streamreader *source) {
    uint8_t value[4];
    streamreader_get_bytes(source, value, 4);
    return ((uint32_t)value[0]) | (((uint32_t)value[1]) << 8) | (((uint32_t)value[2]) << 16) | (((uint32_t)value[3]) << 24);
}

int64_t streamreader_read_int64(streamreader *source) { return (int64_t)streamreader_read_uint64(source); }

uint64_t streamreader_read_uint64(streamreader *source) {
    uint8_t value[8];
    streamreader_get_bytes(source, value, 8);
    return ((uint64_t)value[0]) | (((uint64_t)value[1]) << 8) | (((uint64_t)value[2]) << 16) | (((uint64_t)value[3]) << 24) |
           (((uint64_t)value[3]) << 32) | (((uint64_t)value[3]) << 40) | (((uint64_t)value[3]) << 48) | (((uint64_t)value[3]) << 56);
}

uint64_t streamreader_get_position(const streamreader *source) { return source->position; }

void streamreader_set_position(streamreader *source, uint64_t position) { source->position = position; }

void streamreader_get_bytes(streamreader *source, void *buffer, const uint64_t count) {
    assert(source->position + count < source->data_size);
    memcpy(buffer, &source->data[source->position], count);
    source->position += count;
}

void streamreader_skip_bytes(streamreader *source, const uint64_t count) {
    assert(source->position + count < source->data_size);
    source->position += count;
}

bool streamreader_eof(const streamreader *source) { return source->position >= source->data_size; }
