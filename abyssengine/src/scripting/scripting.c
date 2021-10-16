#include "scripting.h"
#include <lualib.h>

mutex *script_mutex;

void scripting_init() { script_mutex = mutex_create(); }

void scripting_finalize() { mutex_destroy(script_mutex); }

int luaopen_abyss(lua_State *l) {
    lua_register(l, "getConfig", abyss_lua_get_config);
    lua_register(l, "log", abyss_lua_log);
    lua_register(l, "showSystemCursor", abyss_lua_show_system_cursor);
    lua_register(l, "shutdown", abyss_lua_shutdown);
    lua_register(l, "sleep", abyss_lua_sleep);
    // setBootText
    // exitBootMode
    // addLoaderProvider
    // loadString
    // loadSprite
    // loadPalette
    // loadButton
    // loadLabel
    // cursorTexture
    // systemCursorVisible
    // targetFps
    // fullscreen

    return 1;
}
