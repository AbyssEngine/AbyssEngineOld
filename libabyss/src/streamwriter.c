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

#include "streamwriter.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct streamwriter {
    uint8_t *data;
    uint32_t data_size;
    int bit_offset;
    uint8_t bit_cache;
} streamwriter;

streamwriter *streamwriter_create() {
    streamwriter *result = calloc(1, sizeof(streamwriter));
    return result;
}

void streamwriter_destroy(streamwriter *source) {
    assert(source != NULL);

    if (source->data != NULL) {
        free(source->data);
    }

    free(source);
}

const void *streamwriter_get_data(streamwriter *source) {
    assert(source != NULL);

    return (void *)source->data;
}

void streamwriter_push_bytes(streamwriter *source, const void *bytes, uint32_t bytes_len) {
    assert(source != NULL);

    source->data = realloc(source->data, source->data_size + bytes_len);
    memcpy(source->data + bytes_len, bytes, bytes_len);
    source->data_size += bytes_len;
}

void streamwriter_push_bit(streamwriter *source, bool bit) {
    assert(source != NULL);

    if (bit) {
        source->bit_cache |= 1 << source->bit_offset;
    }

    source->bit_offset++;

    if (source->bit_offset != 8) {
        return;
    }

    streamwriter_push_bytes(source, &source->bit_cache, 1);

    source->bit_cache = 0;
    source->bit_offset = 0;
}

void streamwriter_push_bits(streamwriter *source, uint8_t byte, int bits) {
    assert(source != NULL);
    assert(bits <= 8);

    for (int i = 0; i < bits; i++) {
        streamwriter_push_bit(source, (byte & 1) == 1);
        byte >>= 1;
    }
}

void streamwriter_push_bits16(streamwriter *source, uint16_t byte, int bits) {
    assert(source != NULL);
    assert(bits <= 16);

    for (int i = 0; i < bits; i++) {
        streamwriter_push_bit(source, (byte & 1) == 1);
        byte >>= 1;
    }
}

void streamwriter_push_bits32(streamwriter *source, uint32_t byte, int bits) {
    assert(source != NULL);
    assert(bits <= 32);

    for (int i = 0; i < bits; i++) {
        streamwriter_push_bit(source, (byte & 1) == 1);
        byte >>= 1;
    }
}

void streamwriter_push_int16(streamwriter *source, int16_t value) {
    assert(source != NULL);

    streamwriter_push_uint16(source, (uint16_t)value);
}

void streamwriter_push_uint16(streamwriter *source, int16_t value) {
    assert(source != NULL);

    char data[2] = {value & 0xFF, (value >> 8) & 0xFF};
    streamwriter_push_bytes(source, data, 2);
}

void streamwriter_push_int32(streamwriter *source, int32_t value) {
    assert(source != NULL);

    streamwriter_push_uint32(source, (uint32_t)value);
}

void streamwriter_push_uint32(streamwriter *source, int32_t value) {
    assert(source != NULL);

    char data[4] = {value & 0xFF, (value >> 8) & 0xFF, (value >> 16) & 0xFF, (value >> 24) & 0xFF};
    streamwriter_push_bytes(source, data, 4);
}

void streamwriter_push_int64(streamwriter *source, int64_t value) {
    assert(source != NULL);

    streamwriter_push_uint64(source, (uint64_t)value);
}

void streamwriter_push_uint64(streamwriter *source, int64_t value) {
    assert(source != NULL);

    char data[8] = {value & 0xFF,         (value >> 8) & 0xFF,  (value >> 16) & 0xFF, (value >> 24) & 0xFF,
                    (value >> 32) & 0xFF, (value >> 40) & 0xFF, (value >> 48) & 0xFF, (value >> 56) & 0xFF};
    streamwriter_push_bytes(source, data, 8);
}

uint32_t streamwriter_get_size(const streamwriter *source) { return source->data_size; }
