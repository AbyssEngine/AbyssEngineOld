#include "../engine/engine.h"
#include "scripting.h"

int abyss_lua_set_boot_text(lua_State *l) {
    LCHECK_NUMPARAMS(1)
    LCHECK_STRING(1);

    engine_set_boot_text(engine_get_global_instance(), lua_tostring(l, 1));

    return 0;
}
