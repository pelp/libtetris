#ifndef LIBTETRIS_FRAMEBUFFER_H
#define LIBTETRIS_FRAMEBUFFER_H

#include "types.h"

typedef struct {
    uint8_t width;
    uint8_t height;
    piece_id_t *blocks;
} framebuffer_t;

void init_framebuffer(framebuffer_t *framebuffer, uint8_t width, uint8_t height);

#endif
