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

#include "crypto.h"
#include <libabyss/mpqblock.h>
#include <string.h>

bool mpq_block_has_flag(const mpq_block *source, const mpq_block_flag flag) { return (source->flags & flag) != 0; }

uint32_t mpq_block_get_encryption_seed(mpq_block *source, const char *filename) {
    const char *filename_start = strrchr(filename, '\\');
    if (filename_start == 0) {
        filename_start = filename;
    } else {
        filename_start++;
    }
    uint32_t seed = crypto_hash_string(filename_start, 3);
    return (seed + source->file_position) ^ source->file_size_uncompressed;
}
