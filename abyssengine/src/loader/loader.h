#ifndef ABYSS_LOADER_H
#define ABYSS_LOADER_H

#include "loaderprovider.h"
typedef struct loader loader;

loader *loader_new(const char* language_code, const char* language_font_code);
void loader_destroy(loader *src);
void loader_add_provider(loader *src, loader_provider *provider);
void* loader_load(loader *src, const char* path, int *file_size);

#endif // ABYSS_LOADER_H
