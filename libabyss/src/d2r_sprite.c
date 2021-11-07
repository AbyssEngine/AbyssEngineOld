#include "streamreader.h"
#include <assert.h>
#include <libabyss/d2r_sprite.h>
#include <libabyss/log.h>
#include <stdint.h>
#include <stdlib.h>

d2r_sprite *d2r_sprite_new_from_bytes(const void *data, uint64_t size) {
    d2r_sprite *result = calloc(1, sizeof(d2r_sprite));
    streamreader *reader = streamreader_create(data, size);

    streamreader_read_uint32(reader);
    uint16_t version = streamreader_read_uint16(reader);

    if (version != 31) {
        log_fatal("Unsupported sprite version %d", version);
        free(result);
        streamreader_destroy(reader);
        return NULL;
    }

    streamreader_read_uint16(reader);
    result->width = streamreader_read_uint32(reader);
    result->height = streamreader_read_uint32(reader);
    streamreader_read_int32(reader);
    result->frames = streamreader_read_uint32(reader);
    streamreader_read_int32(reader);
    streamreader_read_int32(reader);
    streamreader_read_int32(reader);
    streamreader_read_int32(reader);
    result->data = malloc(result->width * result->height * 4);
    streamreader_get_bytes(reader, result->data, result->width * result->height * 4);

    streamreader_destroy(reader);
    return result;
}

void d2r_sprite_destroy(d2r_sprite *source) {
    free(source->data);
    free(source);
}
