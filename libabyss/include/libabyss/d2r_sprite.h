#ifndef LIBABYSS_D2R_SPRITE_H
#define LIBABYSS_D2R_SPRITE_H

#include <stdint.h>

typedef struct {
    uint32_t frames;
    uint32_t width;
    uint32_t height;
    uint8_t* data;
} d2r_sprite;

d2r_sprite *d2r_sprite_new_from_bytes(const void *data, uint64_t size);
void d2r_sprite_destroy(d2r_sprite *source);

#endif // LIBABYSS_D2R_SPRITE_H
