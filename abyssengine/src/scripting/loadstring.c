#include "../engine/engine.h"
#include "libabyss/utils.h"
#include "scripting.h"

int abyss_lua_load_string(lua_State *l) {
    LCHECK_NUMPARAMS(1)
    LCHECK_STRING(1);

    const char *path = lua_tostring(l, 1);

    char *path_tmp = strdup(path);
    char *path_new = util_fix_mpq_path(path_tmp);
    char *data = loader_load(engine_get_loader(engine_get_global_instance()), path_new, NULL);
    free(path_tmp);

    if (data == NULL) {
        luaL_error(l, "file not found: %s", path);
        return 0;
    }
    lua_pushstring(l, data);
    return 1;
}
