#ifndef LIBTETRIS_TYPES_H
#define LIBTETRIS_TYPES_H

#include <stdint.h>
#include <stdbool.h>

typedef int16_t coord_t;
typedef uint8_t rotation_t;

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

#endif
