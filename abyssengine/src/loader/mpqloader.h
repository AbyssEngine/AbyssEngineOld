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

#ifndef ABYSS_MPQLOADER_H
#define ABYSS_MPQLOADER_H

#include "loaderprovider.h"

typedef struct mpq_loader mpq_loader;

loader_provider *mpq_loader_new(const char *mpq_path);
const char *mpq_loader_get_name(loader_provider *provider);
bool mpq_loader_exists(loader_provider *provider, const char *path);
void *mpq_loader_load(loader_provider *provider, const char *path, int *file_size);
void mpq_loader_destroy(loader_provider *provider);

#endif // ABYSS_MPQLOADER_H
