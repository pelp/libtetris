#ifndef LIBTETRIS_PIECE_H
#define LIBTETRIS_PIECE_H

#include "types.h"

typedef enum : int8_t {
    PIECE_EMPTY = -1,
    PIECE_I = 0,
    PIECE_L = 1,
    PIECE_O = 2,
    PIECE_Z = 3,
    PIECE_T = 4,
    PIECE_J = 5,
    PIECE_S = 6,
    PIECE_COUNT = 7,
    PIECE_GHOST = 8
} piece_id_t;

#define ROTATION_COUNT 4
#define BLOCK_COUNT 4
#define KICK_COUNT 4

typedef coord_t piece_data_t[PIECE_COUNT][ROTATION_COUNT][BLOCK_COUNT];
typedef coord_t kick_table_row_t[KICK_COUNT][2];
typedef kick_table_row_t kick_table_t[ROTATION_COUNT];

static piece_data_t PIECE_DATA_X = {
        {{0, 1, 2, 3}, {2, 2, 2, 2}, {3, 2, 1, 0}, {1, 1, 1, 1}},    // I
        {{2, 2, 1, 0}, {2, 1, 1, 1}, {0, 0, 1, 2}, {0, 1, 1, 1}},    // L
        {{1, 2, 2, 1}, {2, 2, 1, 1}, {2, 1, 1, 2}, {1, 1, 2, 2}},    // O
        {{0, 1, 1, 2}, {2, 2, 1, 1}, {2, 1, 1, 0}, {0, 0, 1, 1}},    // Z
        {{1, 0, 1, 2}, {2, 1, 1, 1}, {1, 2, 1, 0}, {0, 1, 1, 1}},    // T
        {{0, 0, 1, 2}, {2, 1, 1, 1}, {2, 2, 1, 0}, {0, 1, 1, 1}},    // J
        {{2, 1, 1, 0}, {2, 2, 1, 1}, {0, 1, 1, 2}, {0, 0, 1, 1}},    // S
};
static piece_data_t PIECE_DATA_Y = {
        {{1, 1, 1, 1}, {0, 1, 2, 3}, {2, 2, 2, 2}, {3, 2, 1, 0}},    // I
        {{0, 1, 1, 1}, {2, 2, 1, 0}, {2, 1, 1, 1}, {0, 0, 1, 2}},    // L
        {{0, 0, 1, 1}, {0, 1, 1, 0}, {1, 1, 0, 0}, {1, 0, 0, 1}},    // O
        {{0, 0, 1, 1}, {0, 1, 1, 2}, {2, 2, 1, 1}, {2, 1, 1, 0}},    // Z
        {{0, 1, 1, 1}, {1, 0, 1, 2}, {2, 1, 1, 1}, {1, 2, 1, 0}},    // T
        {{0, 1, 1, 1}, {0, 0, 1, 2}, {2, 1, 1, 1}, {2, 2, 1, 0}},    // J
        {{0, 0, 1, 1}, {2, 1, 1, 0}, {2, 2, 1, 1}, {0, 1, 1, 2}},    // S
};

static kick_table_t WALLKICK_NORMAL_CCW = {
        {{1,  0}, {1,  -1}, {0, 2},  {1,  2}},     // 0>>3
        {{1,  0}, {1,  1},  {0, -2}, {1,  -2}},    // 1>>0
        {{-1, 0}, {-1, -1}, {0, 2},  {-1, 2}},     // 2>>1
        {{-1, 0}, {-1, 1},  {0, -2}, {-1, -2}},    // 3>>2
};
static kick_table_t WALLKICK_NORMAL_CW = {
        {{-1, 0}, {-1, -1}, {0, 2},  {-1, 2}},     // 0>>1
        {{1,  0}, {1,  1},  {0, -2}, {1,  -2}},    // 1>>2
        {{1,  0}, {1,  -1}, {0, 2},  {1,  2}},     // 2>>3
        {{-1, 0}, {-1, 1},  {0, -2}, {-1, -2}},    // 3>>0
};

static kick_table_t WALLKICK_I_CCW = {
        {{-1, 0}, {2,  0}, {-1, -2}, {2,  1}},     // 0>>3
        {{2,  0}, {-1, 0}, {2,  -1}, {-1, 2}},     // 1>>0
        {{1,  0}, {-2, 0}, {1,  2},  {-2, -1}},    // 2>>1
        {{-2, 0}, {1,  0}, {-2, 1},  {1,  -2}},    // 3>>2
};
static kick_table_t WALLKICK_I_CW = {
        {{-2, 0}, {1,  0}, {-2, 1},  {1,  -2}},    // 0>>1
        {{-1, 0}, {2,  0}, {-1, -2}, {2,  1}},     // 1>>2
        {{2,  0}, {-1, 0}, {2,  -1}, {-1, 2}},     // 2>>3
        {{1,  0}, {-2, 0}, {1,  2},  {-2, -1}},    // 3>>0
};

static char *PIECE_FRAMEBUFFER[PIECE_COUNT] = {
        "0000",    // I
        "--1\n111", // L
        "22\n22",   // O
        "33-\n-33", // Z
        "-4-\n444", // T
        "5--\n555", // J
        "-66\n66-", // S
};

int get_piece_width(piece_id_t id);

int get_piece_height(piece_id_t id);

coord_t *get_piece_x_coords(piece_id_t id);

coord_t *get_piece_y_coords(piece_id_t id);

#endif