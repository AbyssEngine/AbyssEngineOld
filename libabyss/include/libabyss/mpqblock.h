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

#ifndef LIBABYSS_MPQ_BLOCK_H
#define LIBABYSS_MPQ_BLOCK_H

#include <libabyss/commondef.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint32_t mpq_block_flag;

#define MPQ_BLOCK_FLAG_IMPLODE ((mpq_block_flag)0x00000100)       // File is compressed using PKWARE Data compression library
#define MPQ_BLOCK_FLAG_COMPRESS ((mpq_block_flag)0x00000200)      // File is compressed using combination of compression methods
#define MPQ_BLOCK_FLAG_ENCRYPTED ((mpq_block_flag)0x00010000)     // The file is encrypted
#define MPQ_BLOCK_FLAG_FIX_KEY ((mpq_block_flag)0x00020000)       // The decryption key for the file
#define MPQ_BLOCK_FLAG_PATCH_FILE ((mpq_block_flag)0x00100000)    // The file contains incremental patch for an existing file in base MPQ
#define MPQ_BLOCK_FLAG_SINGLE_UNIT ((mpq_block_flag)0x01000000)   // The file is stored as single unit
#define MPQ_BLOCK_FLAG_DELETE_MARKER ((mpq_block_flag)0x02000000) // File is a deletion marker, indicating that the file no longer exists.
#define MPQ_BLOCK_FLAG_SECTOR_CRC ((mpq_block_flag)0x04000000)    // File has checksums for each sector.
#define MPQ_BLOCK_FLAG_EXISTS ((mpq_block_flag)0x80000000)        // Set if file exists, reset when the file was deleted

typedef PACK(struct mpq_block {
    uint32_t file_position;
    uint32_t file_size_compressed;
    uint32_t file_size_uncompressed;
    mpq_block_flag flags;
}) mpq_block;

bool mpq_block_has_flag(const mpq_block *source, mpq_block_flag flag);
uint32_t mpq_block_get_encryption_seed(mpq_block *source, const char *filename);

#endif // LIBABYSS_MPQ_BLOCK_H
