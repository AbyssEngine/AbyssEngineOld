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
    lua_register(l, "setBootText", abyss_lua_set_boot_text);
    lua_register(l, "addLoaderProvider", abyss_lua_add_loader_provider);
    lua_register(l, "loadString", abyss_lua_load_string);
    // exitBootMode
    // addLoaderProvider
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
