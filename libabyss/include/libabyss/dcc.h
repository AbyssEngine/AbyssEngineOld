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

#ifndef LIBABYSS_DCC_H
#define LIBABYSS_DCC_H

#include "boundsdefs.h"
#include "palette.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct dcc_pixel_buffer_entry {
    uint8_t value[4];
    int frame;
    int frame_cell_index;
} dcc_pixel_buffer_entry;

typedef struct dcc_cell {
    int width;
    int height;
    int offset_x;
    int offset_y;
    int last_width;
    int last_height;
    int last_offset_x;
    int last_offset_y;
} dcc_cell;

typedef struct dcc_frame {
    rectangle box;
    dcc_cell *cells;
    uint8_t *pixel_data;
    int width;
    int height;
    int offset_x;
    int offset_y;
    int number_of_optional_bytes;
    int number_of_coded_bytes;
    int horizontal_cell_count;
    int vertical_cell_count;
    bool frame_is_bottom_up;
    bool is_valid;
} dcc_frame;

typedef struct dcc_direction {
    int out_size_coded;
    int compression_flags;
    int variable_0_bits;
    int width_bits;
    int height_bits;
    int x_offset_bits;
    int y_offset_bits;
    int optional_data_bits;
    int coded_bytes_bits;
    uint32_t equal_cells_bitstream_size;
    uint32_t pixel_mask_bitstream_size;
    uint32_t encoding_type_bitstream_size;
    uint32_t raw_pixel_codes_bytestream_size;
    dcc_frame frames;
    uint8_t palette_entries[256];
    rectangle box;
    dcc_cell *cells;
    uint8_t *pixel_data;
    int horizontal_cell_count;
    int vertical_cell_count;
    dcc_pixel_buffer_entry *pixel_buffer;
} dcc_direction;

typedef struct dcc {
    uint8_t version;
    uint32_t total_size_coded;
    uint32_t number_of_directions;
    uint32_t frames_per_direction;
    dcc_direction *directions;
    const palette *palette;
    bool dirty;
} dcc;

dcc *dcc_new_from_bytes(const void *data, uint64_t size);
void dcc_destroy(dcc *source);

#endif // LIBABYSS_DCC_H
