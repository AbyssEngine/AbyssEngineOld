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

#ifndef ABYSS_BITSTREAM_H
#define ABYSS_BITSTREAM_H

#include <stdint.h>
typedef struct bitstream bitstream;

bitstream *bitstream_create(const void* data, uint32_t size);
void bitstream_destroy(bitstream *source);
int bitstream_read_bits(bitstream *source, int num_bits);
int bitstream_peek_byte(bitstream *source);
void bitstream_waste_bits(bitstream *source, int num_bits);


#endif // ABYSS_BITSTREAM_H
