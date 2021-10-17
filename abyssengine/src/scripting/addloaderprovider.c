#include "../engine/engine.h"
#include "../loader/filesystemloader.h"
#include "../loader/mpqloader.h"
#include "scripting.h"
#include <string.h>

int abyss_lua_add_loader_provider(lua_State *l) {
    LCHECK_NUMPARAMS(2)

    LCHECK_STRING(1);
    LCHECK_STRING(2);

    const char *loader_type = lua_tostring(l, 1);
    const char *loader_path = lua_tostring(l, 2);

    loader_provider *provider;

    if (strcmp(loader_type, "filesystem") == 0) {
        provider = filesystem_loader_new(loader_path);
    } else if (strcmp(loader_type, "mpq") == 0) {
        provider = mpq_loader_new(loader_path);
    } else {
        luaL_error(l, "unknown loader type: %s", loader_type);
        return 0;
    }

    loader_add_provider(engine_get_loader(engine_get_global_instance()), provider);

    return 0;
}
