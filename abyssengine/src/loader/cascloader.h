#ifndef ABYSS_CASCLOADER_H
#define ABYSS_CASCLOADER_H

#include "loaderprovider.h"

#ifdef __cplusplus
extern "C" {
#endif

loader_provider *casc_loader_new(const char *casc_path);

#ifdef __cplusplus
}
#endif

#endif // ABYSS_CASCLOADER_H
