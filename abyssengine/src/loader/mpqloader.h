#ifndef ABYSS_MPQLOADER_H
#define ABYSS_MPQLOADER_H

#include "loaderprovider.h"

typedef struct mpq_loader mpq_loader;

loader_provider *mpq_loader_new(const char *mpq_path);
const char *mpq_loader_get_name(loader_provider *provider);
bool mpq_loader_exists(loader_provider *provider, const char *path);
void *mpq_loader_load(loader_provider *provider, const char *path, int *file_size);
void mpq_loader_destroy(loader_provider *provider);

#endif // ABYSS_MPQLOADER_H
