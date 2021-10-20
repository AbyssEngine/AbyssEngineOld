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
#include "../node/sprite/sprite.h"
#include "scripting.h"
#include <libabyss/palette.h>
#include <libabyss/utils.h>
#include <string.h>

int abyss_lua_load_sprite(lua_State *l) {
    LCHECK_NUMPARAMS(2)
    LCHECK_STRING(1);
    LCHECK_STRING(2);

    const char *path = lua_tostring(l, 1);
    const char *palette_name = lua_tostring(l, 2);

    sprite *result = sprite_load(path, palette_name);

    if (result == NULL) {
        luaL_error(l, "Error loading sprite.\nInvalid file or palette name.");
        return 0;
    }

    lua_pushlightuserdata(l, result);
    return 1;
}
