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

#ifndef ABYSS_ENGINE_H
#define ABYSS_ENGINE_H

#include "../commondef.h"
#include "../loader/loader.h"
#include "../misc/ini.h"
#include "../node/node.h"
#include "libabyss/palette.h"
#include "libabyss/threading.h"
#include "sysfont.h"
#include <SDL2/SDL.h>
#include <libabyss/log.h>
#include <lua.h>
#include <stdbool.h>
#include <stddef.h>

#define GAME_WIDTH 800
#define GAME_HEIGHT 600

typedef struct engine engine;
typedef struct sprite sprite;

#ifndef NDEBUG
extern thread *engine_thread;
#define VERIFY_ENGINE_THREAD                                                                                                                         \
    if (!thread_same(engine_thread)) {                                                                                                               \
        log_fatal("engine called on a non main thread!");                                                                                            \
        exit(-1);                                                                                                                                    \
    }
#else
#define VERIFY_ENGINE_THREAD
#endif

engine *engine_create(char *base_path, ini_file *ini_config);
void engine_destroy(engine *src);
void engine_init_sdl2(engine *src);
void engine_finalize_sdl2(engine *src);
void engine_run(engine *src);
void engine_shutdown(engine *src);
void engine_render(engine *src);
void engine_update(engine *src);
void engine_handle_sdl_event(engine *src, const SDL_Event *evt);
int engine_get_fps(engine *src);
void engine_init_lua(engine *src);
void engine_finalize_lua(engine *src);
sysfont *engine_get_sysfont(const engine *src);
engine *engine_get_global_instance();
void engine_set_global_instance(engine *src);
void engine_show_system_cursor(engine *src, bool show);
SDL_Renderer *engine_get_renderer(engine *src);
ini_file *engine_get_ini_configuration(engine *src);
void engine_set_callbacks(engine *src, void (*render_callback)(engine *src), void (*update_callback)(engine *src, uint32_t tid_diff));
SDL_Texture *engine_get_logo_texture(const engine *src, SDL_Rect *rect);
void engine_set_boot_text(engine *src, const char *boot_text);
const char *engine_get_boot_text(const engine *src);
const char *engine_get_crash_text(const engine *src);
const char *engine_get_base_path(const engine *src);
loader *engine_get_loader(const engine *src);
void engine_trigger_crash(engine *src, const char *crash_text);
void engine_run_script_bootstrap(engine *src);
void engine_exit_boot_mode(engine *src);
const palette *engine_get_palette(const engine *src, const char *palette_name);
bool engine_add_palette(engine *src, const char *palette_name, palette *pal);
void engine_dispatch(engine *src, void (*dispatch)(void *data), void *data);
void engine_set_cursor(engine *src, sprite *cursor, int offset_x, int offset_y);
void engine_get_cursor_position(const engine *src, int *pos_x, int *pos_y);
node *engine_get_root_node(engine *src);
e_mouse_button engine_get_mouse_button_state(const engine *src);
void engine_set_mouse_button_state(engine *src, enum e_mouse_button new_state);
lua_State *engine_get_lua_state(const engine *src);
void engine_play_video(engine *src, const char *path);
bool engine_is_video_playing(const engine *src);
void engine_end_video(engine *src);
void engine_video_mutex_wait(engine *src);
bool engine_get_is_running(const engine *src);
#endif // ABYSS_ENGINE_H
