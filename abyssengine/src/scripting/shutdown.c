#include "../engine/engine.h"
#include "scripting.h"

int abyss_lua_shutdown(lua_State *l) {
    LCHECK_NUMPARAMS(0)

    engine_shutdown(engine_get_global_instance());
    return 0;
}
