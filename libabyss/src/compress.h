#ifndef LIBABYSS_COMPRESS_H
#define LIBABYSS_COMPRESS_H

#include <stdint.h>

void *compress_decompress_wav(void *buffer, uint32_t buffer_size, int channel_count);

#endif //LIBABYSS_COMPRESS_H
