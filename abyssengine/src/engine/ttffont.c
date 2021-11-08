#include "ttffont.h"
#include "../loader/loader.h"
#include "libabyss/utils.h"
#include "engine.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct ttffont {
    TTF_Font* font;
    char* file;
} ttffont;

ttffont *ttffont_load(const char *file_path, int size, int hinting) {
    engine *engine = engine_get_global_instance();
    ttffont *result = calloc(1, sizeof(ttffont));

    int file_size;
    char* new_path = strdup(file_path);
    const char *path_fixed = util_fix_mpq_path(new_path);
    char *data = loader_load(engine_get_loader(engine), path_fixed, &file_size);
    free(new_path);
    result->font = TTF_OpenFontRW(SDL_RWFromConstMem(data, file_size), 1, size);
    if (result->font == NULL) {
        free(result);
        log_fatal("Can't load TTF font: %s", TTF_GetError());
        return NULL;
    }
    result->file = data;
    TTF_SetFontHinting(result->font, hinting);

    return result;
}

void ttffont_destroy(ttffont *source) {
    TTF_CloseFont(source->font);
    free(source->file);
    free(source);
}

SDL_Surface* ttffont_draw_text(ttffont *source, const char *text) {
    SDL_Color color = {255, 255, 255, 0};
    return TTF_RenderText_Blended(source->font, text, color);
}
