#ifndef ABYSS_FILESYSTEM_LOADER_H
#define ABYSS_FILESYSTEM_LOADER_H

#include "loaderprovider.h"

typedef struct filesystem_loader filesystem_loader;

loader_provider *filesystem_loader_new(const char* base_path);
const char *filesystem_loader_get_name(loader_provider *provider);
bool filesystem_loader_exists(loader_provider *provider, const char *path);
void *filesystem_loader_load(loader_provider *provider, const char *path, int *file_size);
void filesystem_loader_destroy(loader_provider *provider);

#endif // ABYSS_FILESYSTEM_LOADER_H
