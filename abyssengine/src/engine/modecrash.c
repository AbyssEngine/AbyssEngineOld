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

#include "modecrash.h"

void engine_render_crash(engine *src) {
    SDL_Renderer *renderer = engine_get_renderer(src);
    sysfont *font = engine_get_sysfont(src);

    SDL_SetRenderDrawColor(renderer, 0x40, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    sysfont_draw_wrap(font, renderer, 100, 100, "AbyssEngine has ran into an error!", 600);
    sysfont_draw_wrap(font, renderer, 100, 150, engine_get_crash_text(src), 600);
}

void engine_update_crash(engine *src, uint32_t tick_diff) {}

void modecrash_set_callbacks(engine *src) { engine_set_callbacks(src, engine_render_crash, engine_update_crash); }
