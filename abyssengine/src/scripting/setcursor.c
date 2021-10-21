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

#include "../node/sprite/sprite.h"
#include "scripting.h"

typedef struct abyss_lua_set_cursor_request {
    int offset_x;
    int offset_y;
    sprite *source;
} abyss_lua_set_cursor_request;

void abyss_lua_set_cursor_dispatch(void *data) {
    abyss_lua_set_cursor_request *request = (abyss_lua_set_cursor_request *)data;
    engine_set_cursor(engine_get_global_instance(), request->source, request->offset_x, request->offset_y);
    free(data);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantFunctionResult"
int abyss_lua_set_cursor(lua_State *l) {
    LCHECK_NUMPARAMS(3)
    // LCHECK_LIGHTUSERDATA(1);
    LCHECK_NUMBER(2);
    LCHECK_NUMBER(3);

    lua_getfield(l, -3, "cid");
    abyss_lua_set_cursor_request *request = malloc(sizeof(abyss_lua_set_cursor_request));
    request->source = (sprite *)lua_touserdata(l, -1);
    request->offset_x = lua_tonumber(l, 2);
    request->offset_y = lua_tonumber(l, 3);

    engine_dispatch(engine_get_global_instance(), abyss_lua_set_cursor_dispatch, (void *)request);

    return 0;
}
#pragma clang diagnostic pop
