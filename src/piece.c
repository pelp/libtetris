#include "piece.h"

framebuffer_t get_piece_framebuffer(piece_id_t id) {
    return PIECE_FRAMEBUFFER[id];
}

uint8_t get_piece_width(piece_id_t id) {
    return PIECE_FRAMEBUFFER[id].width;
}

uint8_t get_piece_height(piece_id_t id) {
    return PIECE_FRAMEBUFFER[id].height;
}

const int8_t *get_piece_blocks(piece_id_t id) {
    return PIECE_FRAMEBUFFER[id].blocks;
}
