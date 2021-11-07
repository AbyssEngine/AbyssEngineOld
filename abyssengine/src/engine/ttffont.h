#ifndef ABYSS_TTFFONT_H
#define ABYSS_TTFFONT_H

#include "../common/blendmode.h"
#include "../common/color.h"
#include <SDL_ttf.h>

typedef struct ttffont ttffont;

ttffont *ttffont_load(const char *file_path, int size);
void ttffont_destroy(ttffont *source);
SDL_Surface* ttffont_draw_text(ttffont *source, const char *text);

#endif // ABYSS_TTFFONT_H
