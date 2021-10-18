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

#ifndef LIBABYSS_STREAMWRITER_H
#define LIBABYSS_STREAMWRITER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct streamwriter streamwriter;

streamwriter *streamwriter_create();
void streamwriter_destroy(streamwriter *source);
const void *streamwriter_get_data(streamwriter *source);
void streamwriter_push_bytes(streamwriter *source, const void *bytes, uint32_t bytes_len);
void streamwriter_push_bit(streamwriter *source, bool bit);
void streamwriter_push_bits(streamwriter *source, uint8_t byte, int bits);
void streamwriter_push_bits16(streamwriter *source, uint16_t byte, int bits);
void streamwriter_push_bits32(streamwriter *source, uint32_t byte, int bits);
void streamwriter_push_int16(streamwriter *source, int16_t value);
void streamwriter_push_uint16(streamwriter *source, int16_t value);
void streamwriter_push_int32(streamwriter *source, int32_t value);
void streamwriter_push_uint32(streamwriter *source, int32_t value);
void streamwriter_push_int64(streamwriter *source, int64_t value);
void streamwriter_push_uint64(streamwriter *source, int64_t value);
uint32_t streamwriter_get_size(const streamwriter *source);

#endif // LIBABYSS_STREAMWRITER_H
