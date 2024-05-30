#ifndef LIBTETRIS_PIECE_H
#define LIBTETRIS_PIECE_H

#include "types.h"
#include "framebuffer.h"

#define ROTATION_COUNT 4
#define BLOCK_COUNT 4
#define KICK_COUNT 4

typedef coord_t piece_data_t[PIECE_COUNT][ROTATION_COUNT][BLOCK_COUNT];
typedef coord_t kick_table_row_t[KICK_COUNT][2];
typedef kick_table_row_t kick_table_t[ROTATION_COUNT];

static const piece_data_t PIECE_DATA_X = {
        {{0, 1, 2, 3}, {2, 2, 2, 2}, {3, 2, 1, 0}, {1, 1, 1, 1}},    // I
        {{2, 2, 1, 0}, {2, 1, 1, 1}, {0, 0, 1, 2}, {0, 1, 1, 1}},    // L
        {{1, 2, 2, 1}, {2, 2, 1, 1}, {2, 1, 1, 2}, {1, 1, 2, 2}},    // O
        {{0, 1, 1, 2}, {2, 2, 1, 1}, {2, 1, 1, 0}, {0, 0, 1, 1}},    // Z
        {{1, 0, 1, 2}, {2, 1, 1, 1}, {1, 2, 1, 0}, {0, 1, 1, 1}},    // T
        {{0, 0, 1, 2}, {2, 1, 1, 1}, {2, 2, 1, 0}, {0, 1, 1, 1}},    // J
        {{2, 1, 1, 0}, {2, 2, 1, 1}, {0, 1, 1, 2}, {0, 0, 1, 1}},    // S
};
static const piece_data_t PIECE_DATA_Y = {
        {{1, 1, 1, 1}, {0, 1, 2, 3}, {2, 2, 2, 2}, {3, 2, 1, 0}},    // I
        {{0, 1, 1, 1}, {2, 2, 1, 0}, {2, 1, 1, 1}, {0, 0, 1, 2}},    // L
        {{0, 0, 1, 1}, {0, 1, 1, 0}, {1, 1, 0, 0}, {1, 0, 0, 1}},    // O
        {{0, 0, 1, 1}, {0, 1, 1, 2}, {2, 2, 1, 1}, {2, 1, 1, 0}},    // Z
        {{0, 1, 1, 1}, {1, 0, 1, 2}, {2, 1, 1, 1}, {1, 2, 1, 0}},    // T
        {{0, 1, 1, 1}, {0, 0, 1, 2}, {2, 1, 1, 1}, {2, 2, 1, 0}},    // J
        {{0, 0, 1, 1}, {2, 1, 1, 0}, {2, 2, 1, 1}, {0, 1, 1, 2}},    // S
};

static const kick_table_t WALLKICK_NORMAL_CCW = {
        {{1,  0}, {1,  -1}, {0, 2},  {1,  2}},     // 0>>3
        {{1,  0}, {1,  1},  {0, -2}, {1,  -2}},    // 1>>0
        {{-1, 0}, {-1, -1}, {0, 2},  {-1, 2}},     // 2>>1
        {{-1, 0}, {-1, 1},  {0, -2}, {-1, -2}},    // 3>>2
};
static const kick_table_t WALLKICK_NORMAL_CW = {
        {{-1, 0}, {-1, -1}, {0, 2},  {-1, 2}},     // 0>>1
        {{1,  0}, {1,  1},  {0, -2}, {1,  -2}},    // 1>>2
        {{1,  0}, {1,  -1}, {0, 2},  {1,  2}},     // 2>>3
        {{-1, 0}, {-1, 1},  {0, -2}, {-1, -2}},    // 3>>0
};

static const kick_table_t WALLKICK_I_CCW = {
        {{-1, 0}, {2,  0}, {-1, -2}, {2,  1}},     // 0>>3
        {{2,  0}, {-1, 0}, {2,  -1}, {-1, 2}},     // 1>>0
        {{1,  0}, {-2, 0}, {1,  2},  {-2, -1}},    // 2>>1
        {{-2, 0}, {1,  0}, {-2, 1},  {1,  -2}},    // 3>>2
};
static const kick_table_t WALLKICK_I_CW = {
        {{-2, 0}, {1,  0}, {-2, 1},  {1,  -2}},    // 0>>1
        {{-1, 0}, {2,  0}, {-1, -2}, {2,  1}},     // 1>>2
        {{2,  0}, {-1, 0}, {2,  -1}, {-1, 2}},     // 2>>3
        {{1,  0}, {-2, 0}, {1,  2},  {-2, -1}},    // 3>>0
};

static piece_id_t PIECE_FRAMEBUFFER_DATA[PIECE_COUNT][6] = {
        [PIECE_I] = {PIECE_I, PIECE_I, PIECE_I, PIECE_I},
        [PIECE_L] = {PIECE_EMPTY, PIECE_EMPTY, PIECE_L, PIECE_L, PIECE_L, PIECE_L},
        [PIECE_O] = {PIECE_O, PIECE_O, PIECE_O, PIECE_O},
        [PIECE_Z] = {PIECE_Z, PIECE_Z, PIECE_EMPTY, PIECE_EMPTY, PIECE_Z, PIECE_Z},
        [PIECE_T] = {PIECE_EMPTY, PIECE_T, PIECE_EMPTY, PIECE_T, PIECE_T, PIECE_T},
        [PIECE_J] = {PIECE_J, PIECE_EMPTY, PIECE_EMPTY, PIECE_J, PIECE_J, PIECE_J},
        [PIECE_S] = {PIECE_EMPTY, PIECE_S, PIECE_S, PIECE_S, PIECE_S, PIECE_EMPTY}
};
static const framebuffer_t PIECE_FRAMEBUFFER[PIECE_COUNT] = {
        [PIECE_I] = {4, 1, PIECE_FRAMEBUFFER_DATA[PIECE_I]},
        [PIECE_L] = {3, 2, PIECE_FRAMEBUFFER_DATA[PIECE_L]},
        [PIECE_O] = {2, 2, PIECE_FRAMEBUFFER_DATA[PIECE_O]},
        [PIECE_Z] = {3, 2, PIECE_FRAMEBUFFER_DATA[PIECE_Z]},
        [PIECE_T] = {3, 2, PIECE_FRAMEBUFFER_DATA[PIECE_T]},
        [PIECE_J] = {3, 2, PIECE_FRAMEBUFFER_DATA[PIECE_J]},
        [PIECE_S] = {3, 2, PIECE_FRAMEBUFFER_DATA[PIECE_S]}
};


uint8_t get_piece_width(piece_id_t id);

uint8_t get_piece_height(piece_id_t id);

const piece_id_t *get_piece_blocks(piece_id_t id);

#endif