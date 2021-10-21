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
#include "scripting.h"
#include <assert.h>

void abyss_lua_show_system_cursor_dispatch(void *data) { engine_show_system_cursor(engine_get_global_instance(), (bool)data); }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantFunctionResult"
int abyss_lua_show_system_cursor(lua_State *l) {

    // It'll _never_ happen, unless I check for this explicitly...
    static_assert(sizeof(int) < sizeof(void *), "pointers must be at least the size of integers.");

    LCHECK_NUMPARAMS(1)
    LCHECK_BOOLEAN(1);

    engine_dispatch(engine_get_global_instance(), abyss_lua_show_system_cursor_dispatch, (void *)(size_t)lua_toboolean(l, 1));
    return 0;
}
#pragma clang diagnostic pop
