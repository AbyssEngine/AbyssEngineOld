#include "../engine/engine.h"
#include "../engine/ttffont.h"
#include "node.h"
#include "scripting.h"

int abyss_lua_ttf_font_destroy(lua_State *l) {
    LCHECK_NUMPARAMS(1)
    SCRIPT_GET_LUA_THIS(source, ttffont)

    ttffont_destroy(source);

    return 0;
}

int abyss_lua_load_ttf_font(lua_State *l) {
    LCHECK_NUMPARAMS(2)
    LCHECK_STRING(1);
    LCHECK_NUMBER(2);

    const char *path = lua_tostring(l, 1);
    int size = lua_tointeger(l, 2);

    ttffont *result = ttffont_load(path, size);

    if (result == NULL) {
        luaL_error(l, "Error loading TTF");
        return 0;
    }

    SCRIPT_CLASS_RESULT_START(result)
    SCRIPT_CLASS_RESULT_PROPERTY("destroy", abyss_lua_ttf_font_destroy)
    ADD_NODE_SCRIPT_CLASS_RESULT_PROPERTIES

    return 1;
}
