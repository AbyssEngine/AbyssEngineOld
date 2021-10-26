#include "cascloader.h"
#include <libabyss/log.h>

#include <CascLib.h>

typedef struct casc_loader {
    loader_provider provider;
    HANDLE hStorage;
    CASC_OPEN_STORAGE_ARGS storage_args;
} casc_loader;

static const char *casc_loader_get_name(loader_provider *provider) { return "CASC Loader"; }

static void casc_loader_destroy(loader_provider *provider) {
    casc_loader *source = (casc_loader *)provider;

    if (source->hStorage) {
        CascCloseStorage(source->hStorage);
    }
    free(source);
}

static bool casc_progress_callback(
        void *PtrUserParam,
        LPCSTR szWork,
        LPCSTR szObject,
        DWORD CurrentValue,
        DWORD TotalValue) {
    if (szObject) {
        log_info("CASC progress: %s (%s), %d/%d", szWork, szObject, CurrentValue, TotalValue);
    } else {
        log_info("CASC progress: %s, %d/%d", szWork, CurrentValue, TotalValue);
    }
    return false;
}

static _Bool casc_loader_exists(loader_provider *provider, const char *path) {
    casc_loader *source = (casc_loader *)provider;
    char *file_path = calloc(1, 4096);
    strcat(file_path, "data:");
    strcat(file_path, path);
    HANDLE file;
    if (CascOpenFile(source->hStorage, file_path, 0, 0, &file)) {
        CascCloseFile(file);
        free(file_path);
        return true;
    }
    free(file_path);
    return false;
}

static void *casc_loader_load(loader_provider *provider, const char *path, int *file_size) {
    casc_loader *source = (casc_loader *)provider;
    char *file_path = calloc(1, 4096);
    strcat(file_path, "data:");
    strcat(file_path, path);
    HANDLE file;
    if (!CascOpenFile(source->hStorage, file_path, 0, 0, &file)) {
        free(file_path);
        return NULL;
    }

    ULONGLONG size;
    CascGetFileSize64(file, &size);

    void *result = malloc(size);
    if (file_size != NULL) {
        *file_size = (int)size;
    }

    CascReadFile(file, result, size, NULL);
    CascCloseFile(file);
    free(file_path);

    return result;
}

loader_provider *casc_loader_new(const char *casc_path) {
    casc_loader *result = calloc(1, sizeof(casc_loader));

    result->provider.load_callback = casc_loader_load;
    result->provider.destroy_callback = casc_loader_destroy;
    result->provider.exists_callback = casc_loader_exists;
    result->provider.get_name_callback = casc_loader_get_name;

    result->storage_args.PfnProgressCallback = casc_progress_callback;
    result->storage_args.PtrProgressParam = result;
    CascOpenStorageEx(casc_path, &result->storage_args, 0, &result->hStorage);

    return (loader_provider *)result;
}
