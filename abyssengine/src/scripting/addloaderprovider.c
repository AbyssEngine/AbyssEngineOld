#include "scripting.h"
#include <string.h>

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
