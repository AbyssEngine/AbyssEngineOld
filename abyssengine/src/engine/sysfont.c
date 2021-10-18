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

#include "sysfont.h"
#include "../misc/util.h"
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
char *strndup(char const *s, size_t n) {
    size_t len = strnlen(s, n);
    char *new = malloc(len + 1);
    if (!new)
        return NULL;
    new[len] = '\0';
    return memcpy(new, s, len);
}
#endif

typedef struct sysfont {
    SDL_Texture *font_texture;
    int width;
    int height;
    int char_width;
    int char_height;
    SDL_Rect *char_rects;
} sysfont;

sysfont *sysfont_create(const void *font_gfx) {
    sysfont *result = malloc(sizeof(sysfont));
    result->char_rects = malloc(sizeof(SDL_Rect) * 256);

    result->font_texture = util_load_texture_png(font_gfx, &result->width, &result->height);
    result->char_width = result->width / 16;
    result->char_height = result->height / 16;

    SDL_SetTextureBlendMode(result->font_texture, SDL_BLENDMODE_BLEND);

    for (int i = 0; i < 256; i++) {
        int tx = i % 16;
        int ty = (i - tx) / 16;
        SDL_Rect *rect = &result->char_rects[i];
        rect->w = result->char_width;
        rect->h = result->char_height;
        rect->x = tx * result->char_width;
        rect->y = ty * result->char_height;
    }

    return result;
}

void sysfont_destroy(sysfont *source) {
    SDL_DestroyTexture(source->font_texture);
    free(source->char_rects);
    free(source);
}

void sysfont_draw(sysfont *source, SDL_Renderer *renderer, const int x, const int y, const char *string) {
    sysfont_draw_wrap(source, renderer, x, y, string, -1);
}

void sysfont_draw_wrap(sysfont *source, SDL_Renderer *renderer, const int x, const int y, const char *string, const int max_width) {
    SDL_Rect target = {x, y, source->char_width, source->char_height};
    SDL_SetTextureColorMod(source->font_texture, 0xFF, 0xFF, 0xFF);

    for (const char *ch = string; *ch != '\0'; ch++) {

        if ((target.x > x) && ((*((char *)(ch - 1)) == ' ') || (*((char *)(ch - 1)) == '\t')) && (max_width > source->char_width)) {
            int temp_x = target.x - x;
            for (const char *ch_temp = ch; ((*ch_temp != ' ') && (*ch_temp != '\t')) && (*ch_temp != '\0'); ch_temp++) {
                temp_x += source->char_width;
            }

            if (temp_x >= max_width) {
                target.x = x;
                target.y += source->char_height;
            }
        }

        if (*ch == '\t') {
            target.x += source->char_width * 3;
            continue;
        }

        if (*ch == '\n') {
            target.x = x;
            target.y += source->char_height;
            continue;
        }

        if ((*ch == '\\') && (*(char *)(ch + 1) == '#')) {
            ch += 2;
            if (strlen(ch) < 7) {
                continue;
            }

            char *hex_str = strndup(ch, 6);
            uint32_t hex_color = strtol(hex_str, NULL, 16);
            free(hex_str);

            SDL_SetTextureColorMod(source->font_texture, (hex_color >> 16) & 0xFF, (hex_color >> 8) & 0xFF, hex_color & 0xFF);
            ch += 6;
            continue;
        }

        SDL_RenderCopy(renderer, source->font_texture, &source->char_rects[*ch], &target);
        target.x += source->char_width;

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
    }
}

int sysfont_get_character_height(const sysfont *source) { return source->height; }

int sysfont_get_character_width(const sysfont *source) { return source->width; }
