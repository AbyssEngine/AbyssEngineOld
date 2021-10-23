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

#include "modeboot.h"
#include "engine.h"
#include "sysfont.h"
#include <config.h>

SDL_Color fade_color;
uint32_t fade_duration;

void engine_render_boot(engine *src) {
    SDL_Renderer *renderer = engine_get_renderer(src);
    sysfont *font = engine_get_sysfont(src);
    SDL_Rect logo_rect;
    SDL_Texture *logo = engine_get_logo_texture(src, &logo_rect);
    SDL_Rect logo_dest = {(GAME_WIDTH / 2) - (logo_rect.w / 2), (GAME_HEIGHT / 2) - logo_rect.h, logo_rect.w, logo_rect.h};

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, logo, &logo_rect, &logo_dest);
    sysfont_draw(font, renderer, logo_dest.x, logo_dest.y + logo_dest.h + 4, "\\#888888 " ABYSS_VERSION_STRING "\nhttps://github.com/AbyssEngine");

    const char *boot_text = engine_get_boot_text(src);
    if (boot_text != NULL) {
        sysfont_draw_wrap(font, renderer, logo_dest.x, logo_dest.y + logo_dest.h + 48, boot_text, GAME_WIDTH - logo_dest.x);
    }

    if (fade_color.a > 0) {
        SDL_SetRenderDrawColor(renderer, fade_color.r, fade_color.g, fade_color.b, fade_color.a);
        SDL_RenderFillRect(renderer, NULL);
    }
}

void engine_update_boot(engine *src, uint32_t tick_diff) {
    if (fade_duration < 2000) {
        fade_duration += tick_diff;
        if (fade_duration >= 2000) {
            fade_duration = 2000;
            fade_color.a = 0;
            engine_run_script_bootstrap(src);
            return;
        }

        fade_color.a = 0xFF - (fade_duration / 8);
    }
}

void modeboot_set_callbacks(engine *src) {
    engine_set_callbacks(src, engine_render_boot, engine_update_boot);
    fade_color.r = fade_color.g = fade_color.b = 0x00;
    fade_color.a = 0xFF;
    fade_duration = 0;
}
