#include "cascloader.h"
#include <libabyss/log.h>
#include <string>
#include <stdbool.h>
#include <CascLib.h>

#ifndef WINAPI
#define WINAPI
#endif // WINAPI

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
    delete source;
}

static bool casc_progress_callback(
        void *PtrUserParam,
        LPCSTR szWork,
        LPCSTR szObject,
        DWORD CurrentValue,
        DWORD TotalValue) {
    if (szObject) {
        log_debug("CASC progress: %s (%s), %d/%d", szWork, szObject, CurrentValue, TotalValue);
    } else {
        log_debug("CASC progress: %s, %d/%d", szWork, CurrentValue, TotalValue);
    }
    return false;
}

static bool WINAPI casc_loader_exists(loader_provider *provider, const char *path) {
    casc_loader *source = (casc_loader *)provider;
    std::string file_path = "data:";
    file_path += path;
    HANDLE file;
    if (CascOpenFile(source->hStorage, file_path.c_str(), 0, 0, &file)) {
        CascCloseFile(file);
        return true;
    }
    return false;
}

static void *casc_loader_load(loader_provider *provider, const char *path, int *file_size) {
    casc_loader *source = (casc_loader *)provider;
    std::string file_path = "data:";
    file_path += path;
    HANDLE file;
    if (!CascOpenFile(source->hStorage, file_path.c_str(), 0, 0, &file)) {
        return NULL;
    }

    DWORD size1 = 0;
    DWORD size2 = CascGetFileSize(file, &size1);
    ULONGLONG size = size1;
    size <<= 32;
    size |= size2;

    void *result = malloc(size);
    if (file_size != NULL) {
        *file_size = (int)size;
    }

    CascReadFile(file, result, size, NULL);
    CascCloseFile(file);

    return result;
}

extern "C" loader_provider *casc_loader_new(const char *casc_path) {
    casc_loader *result = new casc_loader();

    result->provider.load_callback = casc_loader_load;
    result->provider.destroy_callback = casc_loader_destroy;
    result->provider.exists_callback = casc_loader_exists;
    result->provider.get_name_callback = casc_loader_get_name;

    result->storage_args.PfnProgressCallback = casc_progress_callback;
    result->storage_args.PtrProgressParam = result;
    CascOpenStorageEx(casc_path, &result->storage_args, 0, &result->hStorage);

    return (loader_provider *)result;
}
