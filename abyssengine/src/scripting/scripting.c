#include "scripting.h"
#include "../engine/engine.h"
#include "../misc/ini.h"
#include <libabyss/log.h>
#include <lualib.h>

static int abyss_lua_show_system_cursor(lua_State *l) {
    LCHECK_NUMPARAMS(1)

    LCHECK_BOOLEAN(1);

    const bool show = lua_toboolean(l, 1);
    engine_show_system_cursor(engine_get_global_instance(), show);
    return 0;
}

static int abyss_lua_shutdown(lua_State *l) {
    LCHECK_NUMPARAMS(0)

    engine_shutdown(engine_get_global_instance());
    return 0;
}

static int abyss_lua_log(lua_State *l) {
    LCHECK_NUMPARAMS(2)

    LCHECK_STRING(1);
    LCHECK_STRING(2);

    int log_level;
    const char *logType = lua_tostring(l, 1);
    const char *message = lua_tostring(l, 2);

    if (strcmp(logType, "info") == 0) {
        log_level = LOG_INFO;
    } else if (strcmp(logType, "error") == 0) {
        log_level = LOG_ERROR;
    } else if (strcmp(logType, "fatal") == 0) {
        log_level = LOG_FATAL;
    } else if (strcmp(logType, "warn") == 0) {
        log_level = LOG_WARN;
    } else if (strcmp(logType, "debug") == 0) {
        log_level = LOG_DEBUG;
    } else if (strcmp(logType, "trace") == 0) {
        log_level = LOG_TRACE;
    } else {
        log_error("bla");
        luaL_argerror(l, 1, "unexpected log type");
        return 0;
    }

    lua_Debug ar;
    lua_getstack(l, 1, &ar);
    lua_getinfo(l, "nSl", &ar);
    log_log(log_level, &ar.source[1], ar.currentline, "%s", message);

    return 0;
}

static int abyss_lua_add_loader_provider(lua_State *l) {
    LCHECK_NUMPARAMS(2)

    LCHECK_STRING(1);
    LCHECK_STRING(2);

    const char *loader_type = lua_tostring(l, 1);
    const char *loader_path = lua_tostring(l, 2);

    if (strcmp(loader_type, "filesystem") == 0) {

    } else if (strcmp(loader_type, "mpq") == 0) {

    } else {
        luaL_error(l, "unknown loader type: %s", loader_type);
        return 0;
    }
    return 0;
}

static int abyss_lua_get_config(lua_State *l) {
    LCHECK_NUMPARAMS(2)

    LCHECK_STRING(1);
    LCHECK_STRING(2);

    const char *category = lua_tostring(l, 1);
    const char *value = lua_tostring(l, 2);

    ini_file *ini = engine_get_ini_configuration(engine_get_global_instance());
    const char *result = init_file_get_value(ini, category, value);

    if (result == NULL) {
        luaL_error(l, "configuration value '%s' not found for category '%s'", value, category);
        return 0;
    }

    lua_pushstring(l, result);
    return 1;
}

int luaopen_abyss(lua_State *l) {
    lua_register(l, "getConfig", abyss_lua_get_config);
    lua_register(l, "log", abyss_lua_log);
    lua_register(l, "showSystemCursor", abyss_lua_show_system_cursor);
    lua_register(l, "shutdown", abyss_lua_shutdown);
    return 1;
}
