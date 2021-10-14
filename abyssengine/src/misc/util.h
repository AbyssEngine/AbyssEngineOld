#ifndef ABYSS_UTIL_H
#define ABYSS_UTIL_H

#include <SDL2/SDL.h>

void*util_base64_decode(const char* source, unsigned int bytes);
SDL_Texture *util_load_texture_png(const void* source, int *width, int*height);
char *str_replace(char *string, const char *substr, const char *replacement);
char *trim_string(char *str);

#endif // ABYSS_UTIL_H
