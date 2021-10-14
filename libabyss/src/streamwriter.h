#ifndef LIBABYSS_STREAMWRITER_H
#define LIBABYSS_STREAMWRITER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct streamwriter streamwriter;

streamwriter *streamwriter_create();
void streamwriter_destroy(streamwriter *source);
const void *streamwriter_get_data(streamwriter *source);
void streamwriter_push_bytes(streamwriter *source, const void *bytes, uint32_t bytes_len);
void streamwriter_push_bit(streamwriter *source, bool bit);
void streamwriter_push_bits(streamwriter *source, uint8_t byte, int bits);
void streamwriter_push_bits16(streamwriter *source, uint16_t byte, int bits);
void streamwriter_push_bits32(streamwriter *source, uint32_t byte, int bits);
void streamwriter_push_int16(streamwriter *source, int16_t value);
void streamwriter_push_uint16(streamwriter *source, int16_t value);
void streamwriter_push_int32(streamwriter *source, int32_t value);
void streamwriter_push_uint32(streamwriter *source, int32_t value);
void streamwriter_push_int64(streamwriter *source, int64_t value);
void streamwriter_push_uint64(streamwriter *source, int64_t value);
uint32_t streamwriter_get_size(const streamwriter *source);

#endif // LIBABYSS_STREAMWRITER_H
