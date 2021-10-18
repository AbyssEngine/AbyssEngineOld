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
#include "libabyss/utils.h"
#include "scripting.h"

int abyss_lua_load_string(lua_State *l) {
    LCHECK_NUMPARAMS(1)
    LCHECK_STRING(1);

    const char *path = lua_tostring(l, 1);

    char *path_tmp = strdup(path);
    char *path_new = util_fix_mpq_path(path_tmp);
    char *data = loader_load(engine_get_loader(engine_get_global_instance()), path_new, NULL);
    free(path_tmp);

    if (data == NULL) {
        luaL_error(l, "file not found: %s", path);
        return 0;
    }
    lua_pushstring(l, data);
    return 1;
}
