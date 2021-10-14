#ifndef LIBABYSS_STREAMREADER_H
#define LIBABYSS_STREAMREADER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct streamreader streamreader;

streamreader * streamreader_create(const void* data, const uint64_t data_size);
void streamreader_destroy(streamreader *source);
uint8_t streamreader_read_byte(streamreader *source);
int16_t streamreader_read_int16(streamreader *source);
uint16_t streamreader_read_uint16(streamreader *source);
int32_t streamreader_read_int32(streamreader *source);
uint32_t streamreader_read_uint32(streamreader *source);
int64_t streamreader_read_int64(streamreader *source);
uint64_t streamreader_read_uint64(streamreader *source);
uint64_t streamreader_get_position(const streamreader *source);
void streamreader_set_position(streamreader *source, uint64_t position);
void streamreader_get_bytes(streamreader *source, void* buffer, const uint64_t count);
void streamreader_skip_bytes(streamreader *source, const uint64_t count);
bool streamreader_eof(const streamreader *source);

#endif // LIBABYSS_STREAMREADER_H
