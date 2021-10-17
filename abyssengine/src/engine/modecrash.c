#include "modecrash.h"

void engine_render_crash(engine *src) {
    SDL_Renderer *renderer = engine_get_renderer(src);
    sysfont *font = engine_get_sysfont(src);

    SDL_SetRenderDrawColor(renderer, 0x40, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    sysfont_draw_wrap(font, renderer, 100, 100, "AbyssEngine has ran into an error!", 600);
    sysfont_draw_wrap(font, renderer, 100, 150, engine_get_crash_text(src), 600);

    SDL_RenderPresent(renderer);
}

void engine_update_crash(engine *src, uint32_t tick_diff) {}

void modecrashset_callbacks(engine *src) { engine_set_callbacks(src, engine_render_crash, engine_update_crash); }
