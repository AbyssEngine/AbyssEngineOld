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

#include "mpqloader.h"
#include <libabyss/mpq.h>
#include <stdlib.h>
#include <string.h>

typedef struct mpq_loader {
    loader_provider provider;
    mpq *mpq_source;
} mpq_loader;


loader_provider *mpq_loader_new(const char *mpq_path) {
    mpq_loader *result = malloc(sizeof(mpq_loader));

    if (result == NULL)
        return NULL;

    result->provider.load_callback = mpq_loader_load;
    result->provider.destroy_callback = mpq_loader_destroy;
    result->provider.exists_callback = mpq_loader_exists;
    result->provider.get_name_callback = mpq_loader_get_name;
    result->mpq_source = mpq_load(mpq_path);

    return (loader_provider *)result;
}

const char *mpq_loader_get_name(loader_provider *provider) { return "MPQ Loader"; }

bool mpq_loader_exists(loader_provider *provider, const char *path) {
    const mpq_loader *source = (mpq_loader *)provider;

    const bool exists = mpq_file_exists(source->mpq_source, path);

    return exists;
}

void *mpq_loader_load(loader_provider *provider, const char *path, int *file_size) {
    const mpq_loader *source = (mpq_loader *)provider;

    uint32_t f_size;
    void *data = mpq_read_file(source->mpq_source, path, &f_size);

    if (file_size != NULL) {
        *file_size = (int)f_size;
    }

    return data;
}

void mpq_loader_destroy(loader_provider *provider) {
    mpq_loader *source = (mpq_loader *)provider;

    mpq_destroy(source->mpq_source);
    free(source);
}
