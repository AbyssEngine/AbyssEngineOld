#include "moderun.h"

void engine_render_run(engine *src) {
    SDL_Renderer *renderer = engine_get_renderer(src);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void engine_update_run(engine *src, uint32_t tick_diff) {}

void moderun_set_callbacks(engine *src) { engine_set_callbacks(src, engine_render_run, engine_update_run); }
