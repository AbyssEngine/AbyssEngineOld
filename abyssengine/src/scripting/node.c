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

#include "node.h"
#include "../engine/engine.h"
#include "../node/node.h"

int abyss_lua_node_visible(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_THIS(source, node)
        lua_pushboolean(l, source->visible);
        return 1;
    }

    if (lua_gettop(l) == 2) {
        SCRIPT_GET_LUA_THIS(source, node)
        source->visible = lua_toboolean(l, 2);
        return 0;
    }

    lua_error(l);
    return 0;
}

int abyss_lua_node_active(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_THIS(source, node)
        lua_pushboolean(l, source->active);
        return 1;
    }

    if (lua_gettop(l) == 2) {
        SCRIPT_GET_LUA_THIS(source, node)
        source->active = lua_toboolean(l, 2);
        return 0;
    }

    lua_error(l);
    return 0;
}

int abyss_lua_node_position(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_THIS(source, node)
        lua_pushnumber(l, source->x);
        lua_pushnumber(l, source->y);
        return 2;
    }

    if (lua_gettop(l) == 3) {
        SCRIPT_GET_LUA_THIS(source, node)
        source->x = lua_tonumber(l, 2);
        source->y = lua_tonumber(l, 3);
        return 0;
    }

    lua_error(l);
    return 0;
}

typedef struct append_child_dispatch_item {
    node *source;
    node *child;
} append_child_dispatch_item;

void abyss_lua_node_append_child_dispatch(void *data) {
    append_child_dispatch_item *item = (append_child_dispatch_item *)data;
    node_append_child(item->source, item->child);
    free(item);
}

int abyss_lua_node_append_child(lua_State *l) {
    if (lua_gettop(l) != 2) {
        lua_error(l);
        return 0;
    }

    SCRIPT_GET_LUA_THIS(source, node)
    SCRIPT_GET_LUA_CLASS(child, node, 2)

    if (source == child) {
        luaL_error(l, "cannot add a node to itself");
        return 0;
    }

    append_child_dispatch_item *data = malloc(sizeof(append_child_dispatch_item));
    data->source = source;
    data->child = child;
    engine_dispatch(engine_get_global_instance(), abyss_lua_node_append_child_dispatch, data);

    return 0;
}
