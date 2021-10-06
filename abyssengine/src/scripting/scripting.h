#ifndef ABYSS_SCRIPTING_H
#define ABYSS_SCRIPTING_H

#include <lauxlib.h>
#include <lua.h>
#include <stdlib.h>

#define LCHECK_NUMPARAMS(IDX) if (lua_gettop(l) != IDX) { luaL_error(l, "expected %i errors but got %i instead", IDX, lua_gettop(l) ); return 0; }
#define LCHECK_STRING(IDX) luaL_argcheck(l, lua_isstring(l, IDX), IDX, "string expected")
#define LCHECK_BOOLEAN(IDX) luaL_argcheck(l, lua_isboolean(l, IDX), IDX, "boolean expected")

LUALIB_API int luaopen_abyss(lua_State *l);

#endif // ABYSS_SCRIPTING_H
