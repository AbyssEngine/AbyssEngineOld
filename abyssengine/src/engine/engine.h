#ifndef ABYSS_ENGINE_H
#define ABYSS_ENGINE_H

#include "../misc/ini.h"
#include <SDL.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct engine engine;

engine *engine_create();
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
engine *engine_get_global_instance();
void engine_set_global_instance(engine *src);
void engine_show_system_cursor(__attribute__((unused)) engine *src, bool show);
SDL_Renderer *engine_get_renderer(engine *src);
ini_file *engine_get_ini_configuration(engine *src);

#endif // ABYSS_ENGINE_H
