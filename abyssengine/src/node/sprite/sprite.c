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

#include "sprite.h"
#include "../../engine/engine.h"
#include "../../scripting/scripting.h"
#include "../node.h"
#include "libabyss/dc6.h"
#include "libabyss/dcc.h"
#include "libabyss/threading.h"
#include "libabyss/utils.h"
#include <stdlib.h>

typedef struct sprite {
    node node;
    const palette *palette;
} sprite;

sprite *sprite_load(const char *file_path, const char *palette_name) {
    size_t path_len = strlen(file_path) - 4;
    if (path_len < 5) {
        return NULL;
    }

    engine *engine = engine_get_global_instance();

    const palette *palette = engine_get_palette(engine, palette_name);

    if (palette == NULL) {
        return NULL;
    }

    sprite *result = calloc(1, sizeof(sprite));
    result->palette = palette;

    const char *path_end = &file_path[path_len];
    char *path_ext = strdup(path_end);
    for (int i = 1; i < 4; i++)
        path_ext[i] = (char)tolower(path_ext[i]);
    if (strcmp(path_ext, ".dcc") == 0) {
        char *path_tmp = strdup(file_path);
        char *path_new = util_fix_mpq_path(path_tmp);

        int size;
        char *data = loader_load(engine_get_loader(engine_get_global_instance()), path_new, &size);
        free(path_tmp);

        if (data == NULL) {
            free(path_ext);
            free(result);
            return NULL;
        }

        dcc *res = dcc_new_from_bytes(data, size);

        free(path_ext);
        return result;
    } else if (strcmp(path_ext, ".dc6") == 0) {
        char *path_tmp = strdup(file_path);
        char *path_new = util_fix_mpq_path(path_tmp);

        int size;
        char *data = loader_load(engine_get_loader(engine_get_global_instance()), path_new, &size);
        free(path_tmp);

        if (data == NULL) {
            free(path_ext);
            free(result);
            return NULL;
        }

        dc6 *res = dc6_new_from_bytes(data, size);

        free(path_ext);
        return result;
    } else {
        free(result);
        free(path_ext);
        return NULL;
    }
}

void sprite_destroy(sprite *source) { free(source); }
