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
    LCHECK_STRING(1);
    LCHECK_NUMBER(2);

    const char *path = lua_tostring(l, 1);
    int size = lua_tointeger(l, 2);
    int hinting = TTF_HINTING_NORMAL;
    if (lua_gettop(l) >= 3) {
        const char* hinting_str = lua_tostring(l, 3);
        if (strcmp(hinting_str, "normal") == 0) {
            hinting = TTF_HINTING_NORMAL;
        } else if (strcmp(hinting_str, "light") == 0) {
            hinting = TTF_HINTING_LIGHT;
        } else if (strcmp(hinting_str, "mono") == 0) {
            hinting = TTF_HINTING_MONO;
        } else if (strcmp(hinting_str, "none") == 0) {
            hinting = TTF_HINTING_NONE;
        } else {
            luaL_error(l, "unknown hinting");
            return 0;
        }
    }

    ttffont *result = ttffont_load(path, size, hinting);

    if (result == NULL) {
        luaL_error(l, "Error loading TTF");
        return 0;
    }

    SCRIPT_CLASS_RESULT_START(result)
    SCRIPT_CLASS_RESULT_PROPERTY("destroy", abyss_lua_ttf_font_destroy)
    ADD_NODE_SCRIPT_CLASS_RESULT_PROPERTIES

    return 1;
}
