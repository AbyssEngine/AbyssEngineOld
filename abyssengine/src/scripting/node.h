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

#define ADD_NODE_SCRIPT_CLASS_RESULT_PROPERTIES                                                                                                      \
    SCRIPT_CLASS_RESULT_PROPERTY("visible", abyss_lua_node_visible)                                                                                  \
    SCRIPT_CLASS_RESULT_PROPERTY("active", abyss_lua_node_active)                                                                                    \
    SCRIPT_CLASS_RESULT_PROPERTY("position", abyss_lua_node_position)                                                                                \
    SCRIPT_CLASS_RESULT_PROPERTY("appendChild", abyss_lua_node_append_child)

extern int abyss_lua_node_visible(lua_State *l);
extern int abyss_lua_node_active(lua_State *l);
extern int abyss_lua_node_position(lua_State *l);
extern int abyss_lua_node_append_child(lua_State *l);
