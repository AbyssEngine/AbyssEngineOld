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

#include "../engine/engine.h"
#include "../loader/loader.h"
#include "scripting.h"
#include <libabyss/palette.h>
#include <libabyss/utils.h>
#include <string.h>

int abyss_lua_load_palette(lua_State *l) {
    LCHECK_NUMPARAMS(2)
    LCHECK_STRING(1);
    LCHECK_STRING(2);

    const char *palette_name = lua_tostring(l, 1);
    const char *palette_path = lua_tostring(l, 2);

    char *path_tmp = strdup(palette_path);
    char *path_new = util_fix_mpq_path(path_tmp);

    int size;
    char *data = loader_load(engine_get_loader(engine_get_global_instance()), path_new, &size);
    free(path_tmp);

    if (data == NULL) {
        luaL_error(l, "file not found: %s", palette_path);
        return 0;
    }

    palette *pal = palette_new_from_bytes(data, size);
    if (!engine_add_palette(engine_get_global_instance(), palette_name, pal)) {
        luaL_error(l, "Failed to load palette \nPalette name already in use.", palette_name);
    }

    return 0;
}
