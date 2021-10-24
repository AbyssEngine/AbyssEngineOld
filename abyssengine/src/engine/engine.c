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

#include "engine.h"
#include "../loader/filesystemloader.h"
#include "../misc/resources.h"
#include "../misc/util.h"
#include "../node/sprite/sprite.h"
#include "../scripting/scripting.h"
#include "config.h"
#include "libabyss/palette.h"
#include "lua.h"
#include "modeboot.h"
#include "modecrash.h"
#include "moderun.h"
#include "modevideo.h"
#include <lauxlib.h>
#include <libabyss/log.h>
#include <libabyss/threading.h>
#include <libavformat/avformat.h>
#include <lualib.h>
#include <stdlib.h>
#include <string.h>

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif // min

typedef struct dispatch_item {
    void (*func)(void *data);
    void *data;
} dispatch_item;

typedef struct palette_item {
    char *name;
    palette *palette;
} palette_item;

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
    void (*update_callback)(engine *src, uint32_t tick_diff);
    thread *script_thread;
    char *boot_text;
    char *crash_text;
    palette_item *palettes;
    uint32_t num_palettes;
    dispatch_item *dispatches;
    uint32_t num_dispatches;
    mutex *dispatch_mutex;
    mutex *boot_text_mutex;
    mutex *palette_mutex;
    mutex *node_mutex;
    mutex *video_playback_mutex;
    int max_texture_width;
    int max_texture_height;
    sprite *cursor;
    int cursor_x;
    int cursor_y;
    int cursor_offset_x;
    int cursor_offset_y;
    node *root_node;
    e_mouse_button mouse_button_state;
    bool video_playing;
    char *video_file_path;
} engine;

#ifndef NDEBUG
thread *engine_thread;
#endif // NDEBUG

static engine *global_engine_instance;

engine *engine_create(char *base_path, ini_file *ini_config) {
    engine *result = (engine *)calloc(1, sizeof(engine));

    result->dispatch_mutex = mutex_create();
    result->boot_text_mutex = mutex_create();
    result->palette_mutex = mutex_create();
    result->node_mutex = mutex_create();
    result->video_playback_mutex = mutex_create();

    result->pixel_buffer = (uint32_t *)calloc(GAME_WIDTH * GAME_HEIGHT, 4);
    result->base_path = strdup(base_path);
    result->ini_config = ini_config;
    result->run_mode = ENGINE_RUNE_MODE_BOOT;
    result->root_node = malloc(sizeof(node));
    node_initialize(result->root_node);

    // TODO: Make the language settings dynamic
    result->loader = loader_new("eng", "latin");

    engine_init_sdl2(result);
    engine_init_lua(result);
    scripting_init();

    // Store the engine thread so that we can check it if in debug mode
#ifndef NDEBUG
    engine_thread = thread_get_current();
#endif

    return result;
}

void engine_destroy(engine *src) {

    if (src->video_playing) {
        engine_end_video(src);
    }
    scripting_finalize();

    if (src->boot_text != NULL)
        free(src->boot_text);

    if (src->crash_text != NULL)
        free(src->crash_text);

    if (src->palettes != NULL) {
        for (int i = 0; i < src->num_palettes; i++) {
            palette_destroy(src->palettes[i].palette);
            free(src->palettes[i].name);
        }
        free(src->palettes);
    }

    loader_destroy(src->loader);
    sysfont_destroy(src->font);
    free(src->base_path);
    free(src->pixel_buffer);
    mutex_destroy(src->dispatch_mutex);
    mutex_destroy(src->boot_text_mutex);
    mutex_destroy(src->palette_mutex);
    mutex_destroy(src->node_mutex);
    mutex_destroy(src->video_playback_mutex);
    thread_join(src->script_thread);

        engine_finalize_sdl2(src);
    engine_finalize_lua(src);

#ifndef NDEBUG
    free(engine_thread);
#endif

    if (src->video_file_path != NULL)
        free(src->video_file_path);

    free(src);
}

void engine_init_sdl2(engine *src) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        log_fatal(SDL_GetError());
        exit(-1);
    }

    char *window_title = calloc(1, 128);
    sprintf(window_title, "Abyss Engine v%d.%d", ABYSS_VERSION_MAJOR, ABYSS_VERSION_MINOR);
    src->sdl_window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, GAME_WIDTH, GAME_HEIGHT,
                                       SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
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
    src->max_texture_width = render_info.max_texture_width;
    src->max_texture_height = render_info.max_texture_height;

    log_info("Using '%s' graphics rendering API", render_info.name);
    log_info("Max texture size: %dx%d", src->max_texture_width, src->max_texture_height);

    SDL_version sdl_version;
    SDL_GetVersion(&sdl_version);
    log_info("SDL Version: %d.%d.%d", sdl_version.major, sdl_version.minor, sdl_version.patch);

    SDL_RenderSetLogicalSize(src->sdl_renderer, GAME_WIDTH, GAME_HEIGHT);
    SDL_SetRenderDrawBlendMode(src->sdl_renderer, SDL_BLENDMODE_BLEND);
    free(window_title);
}

void engine_finalize_sdl2(engine *src) {
    SDL_DestroyRenderer(src->sdl_renderer);
    SDL_DestroyWindow(src->sdl_window);
    SDL_Quit();
}

static void engine_script_thread_cleanup(void *data) {
    // TODO: Any script cleanup, engine shutdown
    log_info("Bootstrap script execution has completed.");
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantFunctionResult"
static void *engine_script_thread(void *data) {
    engine *src = (engine *)data;

    // thread_cleanup_push(engine_script_thread_cleanup, data);

    scripting_inject_loaders(src->lua_state);

    int file_size;
    char *lua_code = loader_load(src->loader, "bootstrap.lua", &file_size);

    if (lua_code == NULL) {
        engine_trigger_crash(src, "Could not load bootstrap script!");
        return NULL;
    }

    if (luaL_loadbuffer(src->lua_state, lua_code, file_size, "@bootstrap.lua") || lua_pcall(src->lua_state, 0, 0, 0)) {
        const char *crash_text = lua_tostring(src->lua_state, -1);
        log_error(crash_text);
        engine_trigger_crash(src, crash_text);
        return NULL;
    }
    free(lua_code);

    // thread_cleanup_pop(1);

    return NULL;
}
#pragma clang diagnostic pop

void ffmpeg_log_callback(void *avcl, int level, const char *fmt, va_list args) {
    int l;
    switch (level) {
    case AV_LOG_PANIC:
        l = LOG_FATAL;
        break;
    case AV_LOG_ERROR:
        l = LOG_ERROR;
        break;
    case AV_LOG_WARNING:
        l = LOG_WARN;
        break;
    default:
    case AV_LOG_INFO:
        l = LOG_INFO;
        break;
    case AV_LOG_VERBOSE:
        l = LOG_DEBUG;
        break;
    case AV_LOG_TRACE:
        l = LOG_TRACE;
        break;
    }

    log_log(l, __FILE__, __LINE__, fmt, args);
}

void engine_handle_dispatches(engine *src) {
    mutex_lock(src->dispatch_mutex);

    for (int i = 0; i < src->num_dispatches; i++) {
        dispatch_item *item = &src->dispatches[i];
        item->func(item->data);
    }

    src->num_dispatches = 0;

    mutex_unlock(src->dispatch_mutex);
}

void engine_run(engine *src) {
#ifdef NDEBUG
    av_log_set_level(AV_LOG_ERROR);
#else
    av_log_set_level(AV_LOG_INFO);
#endif
    // TODO: The callback breaks sometimes and I don't know why...
    // av_log_set_callback(ffmpeg_log_callback);

    SDL_Event sdl_event;

    src->font = sysfont_create(resource_abyss_system_font);

    loader_add_provider(src->loader, filesystem_loader_new(src->base_path));

    src->texture_logo = util_load_texture_png(resource_abyss_boot_logo, &src->rect_logo.w, &src->rect_logo.h);
    src->last_tick = SDL_GetTicks();
    src->fps = 0;
    src->frame_count_tick = src->last_tick;

    src->is_running = true;

    modeboot_set_callbacks(src);

    if (src->ini_config == NULL) {
        char *crash_message = calloc(1, 4096);
        sprintf(crash_message, "Could not locate ini file at %s.", src->base_path);
        engine_trigger_crash(src, crash_message);
        free(crash_message);
    }

    while (src->is_running) {
        while (SDL_PollEvent(&sdl_event)) {
            engine_handle_sdl_event(src, &sdl_event);
        }

        engine_update(src);
        engine_render(src);
        engine_handle_dispatches(src);
    }
}

void engine_run_script_bootstrap(engine *src) { src->script_thread = thread_create(engine_script_thread, src); }

void engine_handle_sdl_event(engine *src, const SDL_Event *evt) {
    switch (evt->type) {
    case SDL_MOUSEMOTION: {
        src->cursor_x = evt->motion.x;
        src->cursor_y = evt->motion.y;

        mouse_event_info info;
        info.move_event.x = evt->motion.x;
        info.move_event.y = evt->motion.y;

        node_default_mouse_event_callback(src->root_node, src, mouse_event_type_move, &info);
    } break;
    case SDL_MOUSEBUTTONUP: {
        mouse_event_info info;
        info.button_event.pressed = false;

        switch (evt->button.button) {
        case SDL_BUTTON_LEFT:
            info.button_event.button = mouse_button_left;
            src->mouse_button_state &= ~mouse_button_left;
            break;
        case SDL_BUTTON_RIGHT:
            info.button_event.button = mouse_button_right;
            src->mouse_button_state &= ~mouse_button_right;
            break;
        case SDL_BUTTON_MIDDLE:
            info.button_event.button = mouse_button_middle;
            src->mouse_button_state &= ~mouse_button_middle;
            break;
        }

        node_default_mouse_event_callback(src->root_node, src, mouse_event_type_button, &info);
    } break;
    case SDL_MOUSEBUTTONDOWN: {
        mouse_event_info info;
        info.button_event.pressed = true;

        switch (evt->button.button) {
        case SDL_BUTTON_LEFT:
            info.button_event.button = mouse_button_left;
            src->mouse_button_state |= mouse_button_left;
            break;
        case SDL_BUTTON_RIGHT:
            info.button_event.button = mouse_button_right;
            src->mouse_button_state |= mouse_button_right;
            break;
        case SDL_BUTTON_MIDDLE:
            info.button_event.button = mouse_button_middle;
            src->mouse_button_state |= mouse_button_middle;
            break;
        }

        node_default_mouse_event_callback(src->root_node, src, mouse_event_type_button, &info);
    } break;
    case SDL_QUIT:
        engine_shutdown(src);
        return;
    }
}

void engine_shutdown(engine *src) { src->is_running = false; }

void engine_render(engine *src) {
    if (src->render_callback != NULL) {
        mutex_lock(script_mutex);
        mutex_lock(src->boot_text_mutex);
        mutex_lock(src->node_mutex);
        src->render_callback(src);
        mutex_unlock(src->node_mutex);
        mutex_unlock(src->boot_text_mutex);
        mutex_unlock(script_mutex);
    } else {
        SDL_RenderClear(src->sdl_renderer);
    }

    if (src->cursor != NULL) {
        node *n = (node *)src->cursor;
        n->x = src->cursor_x + src->cursor_offset_x;
        n->y = src->cursor_y + src->cursor_offset_y;
        n->render_callback(n, src, 0, 0);
    }
    SDL_RenderPresent(src->sdl_renderer);
}

void engine_update(engine *src) {
    uint32_t new_ticks = SDL_GetTicks();
    uint32_t tick_diff = new_ticks - src->last_tick;
    if (tick_diff <= 0) {
        return;
    }

    src->last_tick = new_ticks;

    if ((new_ticks - src->frame_count_tick) >= 1000) {
        src->frame_count_tick += 1000;
        src->fps = src->frame_count;
        src->frame_count = 1;
    } else {
        src->frame_count++;
    }

    src->update_callback(src, tick_diff);
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

void engine_show_system_cursor(engine *src, bool show) {
    VERIFY_ENGINE_THREAD
    SDL_ShowCursor(show);
}

SDL_Renderer *engine_get_renderer(engine *src) { return src->sdl_renderer; }

ini_file *engine_get_ini_configuration(engine *src) { return src->ini_config; }

sysfont *engine_get_sysfont(const engine *src) { return src->font; }

const char *engine_get_boot_text(const engine *src) { return src->boot_text; }

const char *engine_get_crash_text(const engine *src) { return src->crash_text; }

void engine_set_callbacks(engine *src, void (*render_callback)(engine *src), void (*update_callback)(engine *src, uint32_t tid_diff)) {
    src->render_callback = render_callback;
    src->update_callback = update_callback;
}

const char *engine_get_base_path(const engine *src) { return src->base_path; }

loader *engine_get_loader(const engine *src) { return src->loader; }

SDL_Texture *engine_get_logo_texture(const engine *src, SDL_Rect *rect) {
    if (rect != NULL) {
        *rect = src->rect_logo;
    }
    return src->texture_logo;
}

void engine_set_boot_text(engine *src, const char *boot_text) {
    mutex_lock(src->boot_text_mutex);

    if (boot_text == NULL) {
        if (src->boot_text != NULL) {
            free(src->boot_text);
            src->boot_text = NULL;
            return;
        }

        return;
    }

    src->boot_text = realloc(src->boot_text, strlen(boot_text) + 1);
    memcpy(src->boot_text, boot_text, strlen(boot_text) + 1);

    mutex_unlock(src->boot_text_mutex);
}

void engine_trigger_crash(engine *src, const char *crash_text) {
    // TODO: We should verify thread here, but in a crash scenario is it really worth it?

    src->crash_text = strdup(crash_text);
    modecrash_set_callbacks(src);
}
void engine_exit_boot_mode(engine *src) {
    VERIFY_ENGINE_THREAD

    moderun_set_callbacks(src);
}

const palette *engine_get_palette(const engine *src, const char *palette_name) {
    mutex_lock(src->palette_mutex);

    for (int idx = 0; idx < src->num_palettes; idx++) {
        if (strcmp(src->palettes[idx].name, palette_name) != 0)
            continue;

        palette *result = src->palettes[idx].palette;
        mutex_unlock(src->palette_mutex);
        return result;
    }

    mutex_unlock(src->palette_mutex);
    return NULL;
}

bool engine_add_palette(engine *src, const char *palette_name, palette *pal) {
    const palette *test = engine_get_palette(src, palette_name);
    mutex_lock(src->palette_mutex);
    if (test != NULL) {
        log_fatal("Attempted to add palette '%s', but it already exists.", palette_name);
        return false;
    }

    src->palettes = realloc(src->palettes, sizeof(palette_item) * (++src->num_palettes));
    src->palettes[src->num_palettes - 1].palette = pal;
    src->palettes[src->num_palettes - 1].name = strdup(palette_name);

    mutex_unlock(src->palette_mutex);
    return true;
}

void engine_dispatch(engine *src, void (*dispatch)(void *data), void *data) {
    mutex_lock(src->dispatch_mutex);

    src->dispatches = realloc(src->dispatches, sizeof(dispatch_item) * (src->num_dispatches + 1));
    dispatch_item *entry = &src->dispatches[src->num_dispatches++];
    entry->func = dispatch;
    entry->data = data;

    mutex_unlock(src->dispatch_mutex);
}

void engine_set_cursor(engine *src, sprite *cursor, int offset_x, int offset_y) {
    VERIFY_ENGINE_THREAD

    src->cursor = cursor;
    src->cursor_offset_x = offset_x;
    src->cursor_offset_y = offset_y;
}

node *engine_get_root_node(engine *src) { return src->root_node; }

mutex *engine_get_node_mutex(engine *src) { return src->node_mutex; }

e_mouse_button engine_get_mouse_button_state(const engine *src) { return src->mouse_button_state; }

void engine_set_mouse_button_state(engine *src, enum e_mouse_button new_state) { src->mouse_button_state = new_state; }

void engine_get_cursor_position(const engine *src, int *pos_x, int *pos_y) {
    if (pos_x != NULL)
        *pos_x = src->cursor_x;

    if (pos_y != NULL)
        *pos_y = src->cursor_y;
}
lua_State *engine_get_lua_state(const engine *src) { return src->lua_state; }

void engine_play_video(engine *src, const char *path) {
    VERIFY_ENGINE_THREAD

    mutex_lock(src->video_playback_mutex);
    src->video_playing = true;

    if (src->video_file_path != NULL)
        free(src->video_file_path);

    src->video_file_path = strdup(path);
    modevideo_set_callbacks(src);
    modevideo_load_file(src, path);
}

void engine_end_video(engine *src) {
    VERIFY_ENGINE_THREAD

    src->video_playing = false;
    mutex_unlock(src->video_playback_mutex);
    moderun_set_callbacks(src);
}

void engine_video_mutex_wait(engine *src) {
    mutex_lock(src->video_playback_mutex);
    mutex_unlock(src->video_playback_mutex);
}

bool engine_is_video_playing(const engine *src) { return src->video_playing; }
bool engine_get_is_running(const engine *src) { return src->is_running; }
