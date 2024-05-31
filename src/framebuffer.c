#include <stdlib.h>
#include <string.h>
#include "framebuffer.h"

void init_framebuffer(framebuffer_t *framebuffer, uint8_t width, uint8_t height) {
    framebuffer->width = width;
    framebuffer->height = height;

    size_t tiles_bytes = sizeof(piece_id_t) * width * height;
    if (framebuffer->blocks == NULL) {
        framebuffer->blocks = malloc(tiles_bytes);
    } else {
        framebuffer->blocks = realloc(framebuffer->blocks, tiles_bytes);
    }
    memset(framebuffer->blocks, PIECE_EMPTY, tiles_bytes);
}
