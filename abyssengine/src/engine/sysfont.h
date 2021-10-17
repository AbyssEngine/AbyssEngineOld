#ifndef ABYSS_SYSFONT_H
#define ABYSS_SYSFONT_H

#include <SDL2/SDL.h>
#include <stdint.h>

typedef struct sysfont sysfont;

sysfont *sysfont_create(const void *font_gfx);
void sysfont_destroy(sysfont *source);
void sysfont_draw(sysfont *source, SDL_Renderer *renderer, int x, int y, const char *string);
void sysfont_draw_wrap(sysfont *source, SDL_Renderer *renderer, int x, int y, const char *string, int max_width);
int sysfont_get_character_height(const sysfont *source);
int sysfont_get_character_width(const sysfont *source);

#endif // ABYSS_SYSFONT_H
