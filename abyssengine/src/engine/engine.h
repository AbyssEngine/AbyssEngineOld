#ifndef ABYSS_ENGINE_H
#define ABYSS_ENGINE_H

#include "../loader/loader.h"
#include "../misc/ini.h"
#include "sysfont.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct engine engine;

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
#endif // ABYSS_ENGINE_H
