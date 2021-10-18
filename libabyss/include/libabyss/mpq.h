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
typedef struct mpq mpq;

mpq *mpq_load(const char *file_path);
void mpq_destroy(mpq *source);
bool mpq_file_exists(const mpq *source, const char *filename);
void *mpq_read_file(mpq *source, const char *filename, uint32_t *file_size);
uint32_t mpq_get_block_size(const mpq *source);
FILE *mpq_get_file_stream(const mpq *source);
mpq_block *mpq_get_block(const mpq *source, const char *filename);
uint32_t mpq_get_header_size(const mpq *source);

#endif // LIBABYSS_MPQ_H
