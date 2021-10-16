#include "../engine/engine.h"
#include "scripting.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantFunctionResult"
int abyss_lua_show_system_cursor(lua_State *l) {
    LCHECK_NUMPARAMS(1)
    LCHECK_BOOLEAN(1);

    const bool show = lua_toboolean(l, 1);
    engine_show_system_cursor(engine_get_global_instance(), show);
    return 0;
}
#pragma clang diagnostic pop
