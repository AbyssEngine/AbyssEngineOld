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

#ifndef ABYSS_SCRIPTING_H
#define ABYSS_SCRIPTING_H

#include <lauxlib.h>
#include <libabyss/threading.h>
#include <lua.h>
#include <stdlib.h>

typedef struct engine engine;

#define LCHECK_NUMPARAMS(IDX)                                                                                                                        \
    if (lua_gettop(l) != IDX) {                                                                                                                      \
        luaL_error(l, "expected %i errors but got %i instead", IDX, lua_gettop(l));                                                                  \
        return 0;                                                                                                                                    \
    }
#define LCHECK_STRING(IDX) luaL_argcheck(l, lua_isstring(l, IDX), IDX, "string expected")
#define LCHECK_NUMBER(IDX) luaL_argcheck(l, lua_isnumber(l, IDX), IDX, "number expected")
#define LCHECK_BOOLEAN(IDX) luaL_argcheck(l, lua_isboolean(l, IDX), IDX, "boolean expected")
#define LCHECK_LIGHTUSERDATA(IDX) luaL_argcheck(l, lua_islightuserdata(l, IDX), IDX, "reference to engine object expected")

extern mutex *script_mutex;

void scripting_init();
void scripting_finalize();
extern int abyss_lua_log(lua_State *l);
extern int abyss_lua_get_config(lua_State *l);
extern int abyss_lua_shutdown(lua_State *l);
extern int abyss_lua_show_system_cursor(lua_State *l);
extern int abyss_lua_sleep(lua_State *l);
extern int abyss_lua_set_boot_text(lua_State *l);
extern int abyss_lua_add_loader_provider(lua_State *l);
extern int abyss_lua_load_string(lua_State *l);
extern int abyss_lua_exit_boot_mode(lua_State *l);
extern int abyss_lua_load_palette(lua_State *l);
extern int abyss_lua_load_sprite(lua_State *l);
extern int abyss_lua_set_cursor(lua_State *l);

void scripting_inject_loaders(lua_State *l);

int luaopen_abyss(lua_State *l);

#endif // ABYSS_SCRIPTING_H
