#include "modeboot.h"
#include "engine.h"
#include "sysfont.h"
#include <config.h>

void engine_render_boot(engine *src) {
    SDL_Renderer *renderer = engine_get_renderer(src);
    sysfont *font = engine_get_sysfont(src);
    SDL_Rect logo_rect;
    SDL_Texture *logo = engine_get_logo_texture(src, &logo_rect);
    SDL_Rect logo_dest = {400 - (logo_rect.w / 2), 300 - (logo_rect.h / 2), logo_rect.w, logo_rect.h};

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, logo, &logo_rect, &logo_dest);
    sysfont_draw(font, renderer, logo_dest.x, logo_dest.y + logo_dest.h + 4, "\\#888888 " ABYSS_VERSION_STRING "\nhttps://github.com/AbyssEngine");

    const char *boot_text = engine_get_boot_text(src);
    if (boot_text != NULL) {
        sysfont_draw_wrap(font, renderer, logo_dest.x, logo_dest.y + logo_dest.h + 48, boot_text, 800 - logo_dest.x);
    }

    SDL_RenderPresent(renderer);
}

void modeboot_set_callbacks(engine *src) { engine_set_callbacks(src, engine_render_boot); }
