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

#ifndef ABYSS_LOADER_H
#define ABYSS_LOADER_H

#include "loaderprovider.h"
typedef struct loader loader;

loader *loader_new(const char *language_code, const char *language_font_code);
void loader_destroy(loader *src);
void loader_add_provider(loader *src, loader_provider *provider);
void *loader_load(loader *src, const char *path, int *file_size);

#endif // ABYSS_LOADER_H
