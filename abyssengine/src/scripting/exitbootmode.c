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

void abyss_lua_exit_boot_mode_dispatch(void *data) { engine_exit_boot_mode(engine_get_global_instance()); }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantFunctionResult"
int abyss_lua_exit_boot_mode(lua_State *l) {
    LCHECK_NUMPARAMS(0)

    engine_dispatch(engine_get_global_instance(), abyss_lua_exit_boot_mode_dispatch, NULL);

    return 0;
}
#pragma clang diagnostic pop
