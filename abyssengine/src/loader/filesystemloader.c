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

#include "filesystemloader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

typedef struct filesystem_loader {
    loader_provider provider;
    char *base_path;
} filesystem_loader;

loader_provider *filesystem_loader_new(const char *base_path) {
    filesystem_loader *result = malloc(sizeof(filesystem_loader));
    result->base_path = calloc(1, strlen(base_path) + 1);
    strcat(result->base_path, base_path);

    result->provider.load_callback = filesystem_loader_load;
    result->provider.destroy_callback = filesystem_loader_destroy;
    result->provider.exists_callback = filesystem_loader_exists;
    result->provider.get_name_callback = filesystem_loader_get_name;

    return (loader_provider *)result;
}

const char *filesystem_loader_get_name(loader_provider *provider) { return "FileSystem Loader"; }

bool filesystem_loader_exists(loader_provider *provider, const char *path) {
    filesystem_loader *src = (filesystem_loader *)provider;
    char *file_path = calloc(1, 4096);

    strcat(file_path, src->base_path);
    strcat(file_path, "/");
    strcat(file_path, path);

#ifdef _WIN32
    DWORD file_attr = GetFileAttributesA(file_path);
    bool result = ((file_attr != INVALID_FILE_ATTRIBUTES) && (file_attr != FILE_ATTRIBUTE_DIRECTORY));
#else
    bool result = access(file_path, F_OK) != -1;
#endif

    free(file_path);

    return result;
}

void *filesystem_loader_load(loader_provider *provider, const char *path, int *file_size) {
    filesystem_loader *src = (filesystem_loader *)provider;

    if (!filesystem_loader_exists(provider, path)) {
        return NULL;
    }

    char *file_path = calloc(1, 4096);

    strcat(file_path, src->base_path);
    strcat(file_path, "/");
    strcat(file_path, path);

    FILE *file = fopen(file_path, "rb");
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    rewind(file);

    void *result = malloc(fsize);

    fread(result, fsize, 1, file);
    fclose(file);

    free(file_path);

    if (file_size != NULL) {
        *file_size = (int)fsize;
    }

    return result;
}

void filesystem_loader_destroy(loader_provider *provider) {
    filesystem_loader *src = (filesystem_loader *)provider;
    free(src->base_path);
}
