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

#include "loader.h"
#include "../misc/util.h"
#include "loaderprovider.h"
#include <libabyss/log.h>
#include <string.h>

typedef struct loader {
    loader_provider **providers;
    int num_providers;
} loader;

loader *loader_new() {
    loader *result = calloc(1, sizeof(loader));

    return result;
}

void loader_destroy(loader *src) {
    if (src->providers != NULL) {
        for (int i = 0; i < src->num_providers; i++) {
            free(src->providers[i]);
        }
        free(src->providers);
    }
    free(src);
}

void loader_add_provider(loader *src, loader_provider *provider) {
    src->providers = realloc(src->providers, sizeof(loader_provider *) * (src->num_providers + 1));
    src->providers[src->num_providers++] = provider;
}

void *loader_load(loader *src, const char *path, int *file_size) {
    if (strlen(path) == 0) {
        log_error("empty path supplied");
        return NULL;
    }

    char *new_path = strdup(path);

    new_path = str_replace(new_path, "\\", "/");

    char *path_ptr = new_path;
    if (new_path[0] == '/') {
        path_ptr++;
    }

    void *result = NULL;
    for (int i = 0; i < src->num_providers; i++) {
        if (!loader_provider_exists(src->providers[i], path)) {
            continue;
        }

        result = loader_provider_load(src->providers[i], path, file_size);
        break;
    }

    free(new_path);
    return result;
}

bool loader_file_exists(loader *src, const char *path) {
    bool found = false;
    if (strlen(path) == 0) {
        log_error("empty path supplied");
        return NULL;
    }

    char *new_path = strdup(path);

    new_path = str_replace(new_path, "\\", "/");

    char *path_ptr = new_path;
    if (new_path[0] == '/') {
        path_ptr++;
    }

    for (int i = 0; i < src->num_providers; i++) {
        if (!loader_provider_exists(src->providers[i], path_ptr)) {
            continue;
        }

        found = true;
        break;
    }

    free(new_path);
    return found;
}
