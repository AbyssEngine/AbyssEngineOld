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
#include "../node/sprite/sprite.h"
#include "node.h"
#include "scripting.h"
int abyss_lua_sprite_cell_size(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_THIS(source, sprite)

        int cell_size_x;
        int cell_size_y;
        sprite_get_cell_size(source, &cell_size_x, &cell_size_y);

        lua_pushnumber(l, cell_size_x);
        lua_pushnumber(l, cell_size_y);

        return 2;
    }

    if (lua_gettop(l) != 3) {
        luaL_error(l, "expected cell sizes");
        return 0;
    }

    LCHECK_NUMBER(2);
    LCHECK_NUMBER(3);
    SCRIPT_GET_LUA_THIS(source, sprite)

    sprite_set_cell_size(source, lua_tonumber(l, 2), lua_tonumber(l, 3));

    return 0;
}

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

    SCRIPT_CLASS_RESULT_START(result)
    SCRIPT_CLASS_RESULT_PROPERTY("cellSize", abyss_lua_sprite_cell_size)
    ADD_NODE_SCRIPT_CLASS_RESULT_PROPERTIES

    return 1;
}
