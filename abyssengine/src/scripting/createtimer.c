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

#include "scripting.h"
#include "../engine/engine.h"

typedef struct lua_timer_item {
    uint32_t timer_index;
} lua_timer_item;


int abyss_lua_timer_destroy(lua_State *l) {
    LCHECK_NUMPARAMS(1)
    SCRIPT_GET_LUA_CLASS(source, lua_timer_item, 1)

    engine_timer_remove(engine_get_global_instance(), source->timer_index);

    free(source);

    return 0;
}

int abyss_lua_create_timer(lua_State *l) {
    LCHECK_NUMPARAMS(2)
    LCHECK_NUMBER(1);


    const float rate = lua_tonumber(l, 1);
    const int ref = luaL_ref(l, LUA_REGISTRYINDEX);

    lua_timer_item *result = malloc(sizeof(lua_timer_item));
    result->timer_index = engine_add_timer(engine_get_global_instance(), ref, rate);

    SCRIPT_CLASS_RESULT_START(result)
    SCRIPT_CLASS_RESULT_PROPERTY("destroy", abyss_lua_timer_destroy)

    return 1;
}
