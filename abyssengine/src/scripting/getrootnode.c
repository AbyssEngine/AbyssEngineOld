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
#include "node.h"
#include "scripting.h"

int abyss_lua_get_root_node(lua_State *l) {
    if (lua_gettop(l) != 0) {
        lua_error(l);
        return 0;
    }

    node *result = engine_get_root_node(engine_get_global_instance());

    SCRIPT_CLASS_RESULT_START(result)
    ADD_NODE_SCRIPT_CLASS_RESULT_PROPERTIES

    return 1;
}
