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

#ifndef LIBABYSS_MPQ_H
#define LIBABYSS_MPQ_H

#include "mpqblock.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**
 * A MoPaQ archive.
 */
typedef struct mpq mpq;

/**
 * Loads an instance of a MPQ archive from disk.
 * @param file_path The file path to load the MPQ from.
 * @return A pointer to a mpq if successful.
 * @return NULL if the file was not successfully loaded.
 */
mpq *mpq_load(const char *file_path);

/**
 * Destroys an mpq archive instance.
 * @param source The mpq instance to destroy.
 */
void mpq_destroy(mpq *source);

/**
 * Determines if the specified file exists in a mpq archive.
 * @param source The mpq archive instance to check.
 * @param filename The file path to check within the mpq archive.
 * @return true if the file name specified exists.
 * @return false if the file name does not exist.
 */
bool mpq_file_exists(const mpq *source, const char *filename);

/**
 * Reads a file in a mpq archive and returns the data.
 * @param source  The mpq archive instance to load from.
 * @param filename The path of the file to load.
 * @param[out] file_size If not NULL, the size of the file will be written to this variable.
 * @return If successfully loaded, a pointer to the data loaded.
 * @return If loading failed, NULl is returned.
 */
void *mpq_read_file(mpq *source, const char *filename, uint32_t *file_size);

/**
 * Returns the size of blocks in the mpq archive.
 * @param source The mpq archive instance.
 * @return The size of a block in the mpq archive.
 */
uint32_t mpq_get_block_size(const mpq *source);

/**
 * Returns the underlying file stream for the mpq archive.
 * @param source The mpq archive instance.
 * @return A pointer to the underlying file stream for the MPQ.
 */
FILE *mpq_get_file_stream(const mpq *source);

/**
 * Returns an mpq block based on the specified file name.
 * @param source The mpq archive instance.
 * @param filename The name of the file who's block needs to be obtained.
 * @return A pointer to the mpq block.
 */
mpq_block *mpq_get_block(const mpq *source, const char *filename);

/**
 * Returns the size of the mpq archive's header.
 * @param source The mpq archive instance.
 * @return The size (in bytes) of the mpq header record.
 */
uint32_t mpq_get_header_size(const mpq *source);

#endif // LIBABYSS_MPQ_H
