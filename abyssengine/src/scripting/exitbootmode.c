#include "../engine/engine.h"
#include "scripting.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantFunctionResult"
int abyss_lua_exit_boot_mode(lua_State *l) {
    LCHECK_NUMPARAMS(0)
    engine_exit_boot_mode(engine_get_global_instance());

    return 0;
}
#pragma clang diagnostic pop
