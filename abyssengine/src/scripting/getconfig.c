#include "../engine/engine.h"
#include "scripting.h"

int abyss_lua_get_config(lua_State *l) {
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
