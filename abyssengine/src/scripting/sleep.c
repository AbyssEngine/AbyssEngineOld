#include "scripting.h"
#include <SDL2/SDL.h>

int abyss_lua_sleep(lua_State *l) {
    LCHECK_NUMPARAMS(1)
    LCHECK_STRING(1);

    const int sleep_msec = lua_tointeger(l, 1);

    SDL_Delay(sleep_msec);

    return 0;
}
