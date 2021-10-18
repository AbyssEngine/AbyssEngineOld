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

#ifndef LIBABYSS_UTILS_H
#define LIBABYSS_UTILS_H

#include <stdbool.h>

char *util_fix_mpq_path(char *path);
void util_get_folder_path_part(char *path);
void util_normalize_path(char *path);
const char *util_get_cwd();
void util_create_directory(const char *path);
void util_create_directories_recursively(const char *path);
char *util_trim_string(char *str);
void util_trim_end_string(char *str);
bool util_is_space(unsigned char ch);

#endif // LIBABYSS_UTILS_H
