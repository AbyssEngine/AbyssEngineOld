#include "scripting.h"
#include "../engine/engine.h"
#include "libabyss/log.h"
#include <lualib.h>

mutex *script_mutex;

void scripting_init() { script_mutex = mutex_create(); }

int scripting_loader(lua_State *l) {
    const char *original_path = lua_tostring(l, 1);
    if (strlen(original_path) == 0) {
        return 0;
    }

    char *path = calloc(1, 4096);
    if (original_path[0] != '/' && original_path[0] != '\\') {
        strcat(path, "/");
    }

    strcat(path, original_path);
    strcat(path, ".lua");

    loader *loader = engine_get_loader(engine_get_global_instance());
    int file_size;
    char *value = loader_load(loader, path, &file_size);

    if (value == NULL) {
        lua_pushstring(l, "Module not found.");
        return 1;
    }

    if (luaL_loadbuffer(l, value, file_size, path)) {
        engine *src = engine_get_global_instance();
        const char *crash_text = lua_tostring(l, -1);
        log_error(crash_text);
        lua_pop(l, 1);
        engine_trigger_crash(src, crash_text);
    }

    free(path);
    free(value);
    return 1;
}

void scripting_inject_loaders(lua_State *l) {
    lua_register(l, "abyss_loader", scripting_loader);
    luaL_dostring(l, "package.searchers = {abyss_loader}");
}

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
