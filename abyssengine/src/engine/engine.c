#include "engine.h"
#include "../loader/filesystemloader.h"
#include "../loader/loader.h"
#include "../misc/resources.h"
#include "../misc/util.h"
#include "../scripting/scripting.h"
#include "config.h"
#include "lua.h"
#include "modeboot.h"
#include <lauxlib.h>
#include <libabyss/log.h>
#include <libabyss/threading.h>
#include <lualib.h>
#include <stdlib.h>
#include <string.h>

typedef int engine_run_mode;

enum { ENGINE_RUNE_MODE_BOOT };

typedef struct engine {
    bool is_running;
    uint32_t last_tick;
    uint32_t frame_count_tick;
    int fps, frame_count;
    SDL_Window *sdl_window;
    SDL_Renderer *sdl_renderer;
    uint32_t *pixel_buffer;
    lua_State *lua_state;
    SDL_Texture *texture_logo;
    SDL_Rect rect_logo;
    loader *loader;
    char *base_path;
    ini_file *ini_config;
    sysfont *font;
    engine_run_mode run_mode;
    void (*render_callback)(engine *src);
    thread *script_thread;
} engine;

static engine *global_engine_instance;

engine *engine_create(char *base_path, ini_file *ini_config) {
    engine *result = (engine *)calloc(1, sizeof(engine));

    result->pixel_buffer = (uint32_t *)calloc(800 * 600, 4);
    result->base_path = strdup(base_path);
    result->ini_config = ini_config;
    result->run_mode = ENGINE_RUNE_MODE_BOOT;

    // TODO: Make the language settings dynamic
    result->loader = loader_new("eng", "latin");

    engine_init_sdl2(result);
    engine_init_lua(result);
    scripting_init();

    return result;
}

void engine_destroy(engine *src) {
    scripting_finalize();
    engine_finalize_sdl2(src);
    engine_finalize_lua(src);

    loader_destroy(src->loader);
    sysfont_destroy(src->font);
    free(src->base_path);
    free(src->pixel_buffer);
    free(src);
}

void engine_init_sdl2(engine *src) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        log_fatal(SDL_GetError());
        exit(-1);
    }

    char *window_title = calloc(1, 128);
    sprintf(window_title, "Abyss Engine v%d.%d", ABYSS_VERSION_MAJOR, ABYSS_VERSION_MINOR);
    src->sdl_window =
        SDL_CreateWindow(window_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (src->sdl_window == 0) {
        log_fatal(SDL_GetError());
        exit(-1);
    }
    src->sdl_renderer = SDL_CreateRenderer(src->sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (src->sdl_renderer == 0) {
        log_fatal(SDL_GetError());
        exit(-1);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

    if (src->sdl_renderer == 0) {
        log_fatal(SDL_GetError());
        exit(-1);
    }

    SDL_RendererInfo render_info;
    SDL_GetRendererInfo(src->sdl_renderer, &render_info);

    log_info("Using '%s' graphics rendering API", render_info.name);

    SDL_RenderSetLogicalSize(src->sdl_renderer, 800, 600);

    free(window_title);
}

void engine_finalize_sdl2(engine *src) {
    SDL_DestroyRenderer(src->sdl_renderer);
    SDL_DestroyWindow(src->sdl_window);
    SDL_Quit();
}

static void engine_script_thread_cleanup(void *data) {
    // TODO: Any script cleanup, engine shutdown
    log_info("Bootstrap script executed has completed.");
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantFunctionResult"
static void *engine_script_thread(void *data) {
    engine *src = (engine *)data;

    thread_cleanup_push(engine_script_thread_cleanup, data);

    int file_size;
    char *lua_code = loader_load(src->loader, "test.lua", &file_size);

    if (lua_code == NULL) {
        log_fatal("Could not load bootstrap script!");
        return NULL;
    }

    if (luaL_loadbuffer(src->lua_state, lua_code, file_size, "@test.lua") || lua_pcall(src->lua_state, 0, 0, 0)) {
        log_error(lua_tostring(src->lua_state, -1));
        return NULL;
    }
    free(lua_code);

    thread_cleanup_pop(1);

    return NULL;
}
#pragma clang diagnostic pop

void engine_run(engine *src) {
    SDL_Event sdl_event;

    src->font = sysfont_create(resource_abyss_system_font);

    loader_add_provider(src->loader, filesystem_loader_new("./"));

    src->texture_logo = util_load_texture_png(resource_abyss_boot_logo, &src->rect_logo.w, &src->rect_logo.h);
    src->last_tick = SDL_GetTicks();
    src->fps = 0;
    src->frame_count_tick = src->last_tick;

    src->is_running = true;

    src->script_thread = thread_create(engine_script_thread, src);

    modeboot_set_callbacks(src);

    while (src->is_running) {
        while (SDL_PollEvent(&sdl_event)) {
            engine_handle_sdl_event(src, &sdl_event);
        }

        engine_update(src);
        engine_render(src);
    }
}

void engine_handle_sdl_event(engine *src, const SDL_Event *evt) {
    switch (evt->type) {
    case SDL_QUIT:
        engine_shutdown(src);
        return;
    }
}

void engine_shutdown(engine *src) { src->is_running = false; }

void engine_render(engine *src) {
    if (src->render_callback != NULL) {
        mutex_lock(script_mutex);
        src->render_callback(src);
        mutex_unlock(script_mutex);
        return;
    }
    SDL_RenderClear(src->sdl_renderer);
    SDL_RenderPresent(src->sdl_renderer);
}

void engine_update(engine *src) {
    uint32_t new_ticks = SDL_GetTicks();
    uint32_t tick_diff = new_ticks - src->last_tick;
    if (tick_diff <= 0) {
        return;
    }
    mutex_lock(script_mutex);
    
    src->last_tick = new_ticks;

    if ((new_ticks - src->frame_count_tick) >= 1000) {
        src->frame_count_tick += 1000;
        src->fps = src->frame_count;
        src->frame_count = 1;
    } else {
        src->frame_count++;
    }

    mutex_unlock(script_mutex);
}

int engine_get_fps(engine *src) { return src->fps; }

void engine_init_lua(engine *src) {
    src->lua_state = luaL_newstate();
    luaL_openlibs(src->lua_state);
    luaopen_abyss(src->lua_state);
}

void engine_finalize_lua(engine *src) { lua_close(src->lua_state); }

engine *engine_get_global_instance() { return global_engine_instance; }

void engine_set_global_instance(engine *src) { global_engine_instance = src; }

void engine_show_system_cursor(engine *src, bool show) { SDL_ShowCursor(show); }

SDL_Renderer *engine_get_renderer(engine *src) { return src->sdl_renderer; }

ini_file *engine_get_ini_configuration(engine *src) { return src->ini_config; }

sysfont *engine_get_sysfont(const engine *src) { return src->font; }

void engine_set_callbacks(engine *src, void (*render_callback)(engine *src)) { src->render_callback = render_callback; }

SDL_Texture *engine_get_logo_texture(const engine *src, SDL_Rect *rect) {
    if (rect != NULL) {
        *rect = src->rect_logo;
    }
    return src->texture_logo;
}
