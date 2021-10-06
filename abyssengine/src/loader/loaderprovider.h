#ifndef ABYSS_LOADER_PROVIDER_H
#define ABYSS_LOADER_PROVIDER_H

#include <stdbool.h>

typedef struct loader_provider loader_provider;
typedef struct loader_provider {
    const char *(*get_name_callback)(loader_provider *provider);
    bool (*exists_callback)(loader_provider *provider, const char *path);
    void *(*load_callback)(loader_provider *provider, const char *path, int *file_size);
    void (*destroy_callback)(loader_provider *provider);
} loader_provider;

const char *loader_provider_get_name(loader_provider *provider);
bool loader_provider_exists(loader_provider *provider, const char *path);
void *loader_provider_load(loader_provider *provider, const char *path, int *file_size);
void loader_provider_destroy(loader_provider *provider);

#endif // ABYSS_LOADER_PROVIDER_H
