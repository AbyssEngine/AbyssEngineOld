/**
 * Copyright (C) 2021 Tim Sarbin
 * This file is part of AbyssEngine <https://github.com/AbyssEngine>.
 *
 * AbyssEngine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AbyssEngine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with AbyssEngine.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../engine/engine.h"
#include "scripting.h"
#include <lua.h>
#include <stdbool.h>

void abyss_lua_play_video_dispatch(void *data) {
    engine_play_video(engine_get_global_instance(), (char *)data);
    free(data);
}

int abyss_lua_play_video(lua_State *l) {
    LCHECK_NUMPARAMS(2)
    LCHECK_STRING(1);
    LCHECK_BOOLEAN(2);

    const char *path = lua_tostring(l, 1);
    const bool wait = lua_toboolean(l, 2);

    engine_dispatch(engine_get_global_instance(), abyss_lua_play_video_dispatch, strdup(path));

    if (wait) {
        engine *e = engine_get_global_instance();
        // do a hard wait for the video to start...
        while (!engine_is_video_playing(e)) {
            SDL_Delay(1);
        }

        // now do a soft mutex wait for the video to stop
        engine_video_mutex_wait(e);

        if (!engine_get_is_running(e))
            luaL_error(l, "engine is shutting down");
    }

    return 0;
}
