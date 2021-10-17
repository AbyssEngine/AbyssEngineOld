#include "mpqloader.h"
#include <libabyss/mpq.h>
#include <stdlib.h>
#include <string.h>

typedef struct mpq_loader {
    loader_provider provider;
    char *mpq_path;
} mpq_loader;

loader_provider *mpq_loader_new(const char *mpq_path) {
    mpq_loader *result = malloc(sizeof(mpq_loader));

    result->provider.load_callback = mpq_loader_load;
    result->provider.destroy_callback = mpq_loader_destroy;
    result->provider.exists_callback = mpq_loader_exists;
    result->provider.get_name_callback = mpq_loader_get_name;
    result->mpq_path = strdup(mpq_path);

    return (loader_provider *)result;
}

const char *mpq_loader_get_name(loader_provider *provider) { return "MPQ Loader"; }

bool mpq_loader_exists(loader_provider *provider, const char *path) {
    mpq_loader *source = (mpq_loader *)provider;
    mpq *m = mpq_load(source->mpq_path);
    if (m == NULL) {
        return false;
    }

    bool exists = mpq_file_exists(m, path);
    mpq_destroy(m);
    return exists;
}

void *mpq_loader_load(loader_provider *provider, const char *path, int *file_size) {
    mpq_loader *source = (mpq_loader *)provider;
    mpq *m = mpq_load(source->mpq_path);
    if (m == NULL) {
        return NULL;
    }

    uint32_t f_size;
    void *data = mpq_read_file(m, path, &f_size);

    if (file_size != NULL) {
        *file_size = (int)f_size;
    }

    mpq_destroy(m);

    return data;
}

void mpq_loader_destroy(loader_provider *provider) {
    mpq_loader *source = (mpq_loader *)provider;

    free(source->mpq_path);
    free(source);
}
