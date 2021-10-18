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

#ifndef ABYSS_INI_H
#define ABYSS_INI_H

typedef struct ini_file_entry {
    char *name;
    char *value;
} ini_file_entry;

typedef struct ini_file_category {
    char *name;
    ini_file_entry **entries;
    int num_entries;
} ini_file_category;

typedef struct ini_file {
    ini_file_category **categories;
    int num_categories;
} ini_file;

ini_file *ini_file_load(const char *file_path);
void ini_file_destroy(ini_file *source);
ini_file_category *ini_file_add_category(ini_file *source, const char *category_name);
ini_file_category *ini_file_get_category(ini_file *source, const char *category_name);
ini_file_entry *ini_file_add_entry(ini_file_category *source, const char *name, const char *value);
void ini_file_destroy_category(ini_file_category *source);
void ini_file_destroy_entry(ini_file_entry *source);
ini_file_entry *ini_file_get_entry(ini_file_category *source, const char *name);
const char *init_file_get_value(ini_file *source, const char *category, const char *name);
#endif // ABYSS_INI_H
