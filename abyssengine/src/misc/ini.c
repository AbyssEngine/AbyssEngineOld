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

#include "ini.h"
#include "util.h"
#include <libabyss/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
size_t getdelim(char **buf, size_t *bufsiz, int delimiter, FILE *fp) {
    char *ptr, *eptr;

    if (*buf == NULL || *bufsiz == 0) {
        *bufsiz = BUFSIZ;
        if ((*buf = malloc(*bufsiz)) == NULL)
            return 0;
    }

    for (ptr = *buf, eptr = *buf + *bufsiz;;) {
        int c = fgetc(fp);
        if (c == -1) {
            if (feof(fp)) {
                size_t diff = (size_t)(ptr - *buf);
                if (diff != 0) {
                    *ptr = '\0';
                    return diff;
                }
            }
            return 0;
        }
        *ptr++ = c;
        if (c == delimiter) {
            *ptr = '\0';
            return ptr - *buf;
        }
        if (ptr + 2 >= eptr) {
            char *nbuf;
            size_t nbufsiz = *bufsiz * 2;
            size_t d = ptr - *buf;
            if ((nbuf = realloc(*buf, nbufsiz)) == NULL)
                return 0;
            *buf = nbuf;
            *bufsiz = nbufsiz;
            eptr = nbuf + nbufsiz;
            ptr = nbuf + d;
        }
    }
}
size_t getline(char **buf, size_t *bufsiz, FILE *fp) { return getdelim(buf, bufsiz, '\n', fp); }
#endif

ini_file *ini_file_load(const char *file_path) {
    ini_file *result = calloc(1, sizeof(ini_file));

    FILE *file;

    file = fopen(file_path, "r");

    if (file == NULL) {
        free(result);
        return NULL;
    }

    size_t line_size = 0;
    char *line = NULL;
    ini_file_category *current_category = NULL;
    int line_idx = 0;

    while (getline(&line, &line_size, file) > 0) {
        line_idx++;

        if (line_size == 0) {
            continue;
        }

        char *line_trimmed = trim_string(line);

        if (strlen(line_trimmed) == 0) {
            continue;
        }

        if (line_trimmed[0] == '[') {
            if (line_trimmed[strlen(line_trimmed) - 1] != ']') {
                log_error("Malformed category header on line %i of %s.", line_idx, file_path);
                ini_file_destroy(result);
                fclose(file);
                free(line_trimmed);
                return NULL;
            }

            line_trimmed[strlen(line_trimmed) - 1] = 0;
            line_trimmed = &line_trimmed[1];

            current_category = ini_file_get_category(result, line_trimmed);
            if (current_category != NULL) {
                continue;
            }

            current_category = ini_file_add_category(result, line_trimmed);
            continue;
        }

        if (current_category == NULL) {
            log_error("Value not in a category on line %i of %s.", line_idx, file_path);
            ini_file_destroy(result);
            fclose(file);
            free(line_trimmed);
            return NULL;
        }

        char *field_name = line_trimmed;
        char *field_value = strchr(line_trimmed, '=');

        if (field_value == NULL) {
            log_error("Malformed value on line %i of %s.", line_idx, file_path);
            ini_file_destroy(result);
            fclose(file);
            free(line_trimmed);
            return NULL;
        }
        field_value++;
        *(field_value - 1) = 0;
        ini_file_add_entry(current_category, field_name, field_value);
    }

    free(line);
    fclose(file);

    return result;
}

void ini_file_destroy(ini_file *source) {
    for (int category_idx = 0; category_idx < source->num_categories; category_idx++) {
        ini_file_destroy_category(source->categories[category_idx]);
    }

    free(source->categories);
    free(source);
}

ini_file_category *ini_file_add_category(ini_file *source, const char *category_name) {
    ini_file_category *result = calloc(1, sizeof(ini_file_category));

    result->name = strdup(category_name);

    source->categories = realloc(source->categories, sizeof(ini_file_category *) * (++source->num_categories));
    source->categories[source->num_categories - 1] = result;

    return result;
}

ini_file_category *ini_file_get_category(ini_file *source, const char *category_name) {
    for (int i = 0; i < source->num_categories; i++) {
        if (strcmp(source->categories[i]->name, category_name) != 0) {
            continue;
        }

        return source->categories[i];
    }

    return NULL;
}

ini_file_entry *ini_file_add_entry(ini_file_category *source, const char *name, const char *value) {
    ini_file_entry *result = calloc(1, sizeof(ini_file_entry));

    result->name = strdup(name);
    result->value = strdup(value);

    source->entries = realloc(source->entries, sizeof(ini_file_entry *) * (++source->num_entries));
    source->entries[source->num_entries - 1] = result;

    return result;
}

void ini_file_destroy_category(ini_file_category *source) {
    free(source->name);
    for (int i = 0; i < source->num_entries; i++) {
        ini_file_destroy_entry(source->entries[i]);
    }
    free(source->entries);
    free(source);
}

void ini_file_destroy_entry(ini_file_entry *source) {
    free(source->name);
    free(source->value);
    free(source);
}

ini_file_entry *ini_file_get_entry(ini_file_category *source, const char *name) {
    for (int i = 0; i < source->num_entries; i++) {
        ini_file_entry *entry = source->entries[i];
        if (strcmp(entry->name, name) != 0) {
            continue;
        }

        return entry;
    }

    return NULL;
}

const char *init_file_get_value(ini_file *source, const char *category, const char *name) {
    ini_file_category *cat = ini_file_get_category(source, category);

    if (cat == NULL) {
        return NULL;
    }

    ini_file_entry *entry = ini_file_get_entry(cat, name);

    if (entry == NULL) {
        return NULL;
    }

    return entry->value;
}
