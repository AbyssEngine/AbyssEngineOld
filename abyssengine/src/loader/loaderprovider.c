#include "loaderprovider.h"

const char *loader_provider_get_name(loader_provider *provider) { return provider->get_name_callback(provider); }

bool loader_provider_exists(loader_provider *provider, const char *path) { return provider->exists_callback(provider, path); }

void *loader_provider_load(loader_provider *provider, const char *path, int *file_size) { return provider->load_callback(provider, path, file_size); }

void loader_provider_destroy(loader_provider *provider) { provider->destroy_callback(provider); }
