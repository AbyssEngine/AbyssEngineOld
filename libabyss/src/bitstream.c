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

#include "bitstream.h"
#include <stdbool.h>
#include <stdlib.h>

typedef struct bitstream {
   const uint8_t *data;
   uint32_t data_size;
   int data_position;
   int current;
   int bit_count;
} bitstream;

bool ensure_bits(bitstream *source, int num_bits) {
   if (num_bits <= source->bit_count)
       return true;

   if (source->data_position >= source->data_size)
       return false;

   source->current |= ((int)source->data[source->data_position++]) << source->bit_count;
   source->bit_count += 8;

   return true;
}

bitstream *bitstream_create(const void *data, uint32_t size) {
   bitstream *result = calloc(1, sizeof(bitstream));
   result->data = (uint8_t *)data;
   result->data_size = size;
   return result;
}

void bitstream_destroy(bitstream *source) { free(source); }

int bitstream_read_bits(bitstream *source, int num_bits) {
   if (!ensure_bits(source, num_bits))
       return -1;

   int result = source->current & (0xffff >> (unsigned int)(16 - num_bits));

   bitstream_waste_bits(source, num_bits);

   return result;
}

int bitstream_peek_byte(bitstream *source) {
   if (ensure_bits(source, 8))
       return -1;

   return source->current & 0xff;
}

void bitstream_waste_bits(bitstream *source, int num_bits) {
   source->current >>= num_bits;
   source->bit_count -= num_bits;
}
