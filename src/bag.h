#ifndef LIBTETRIS_BAG_H
#define LIBTETRIS_BAG_H

#include "piece.h"

#define NUM_NEXT_PIECES 5

typedef struct {
    PIECE_ID current;
    PIECE_ID order[PIECE_COUNT];
    PIECE_ID next[NUM_NEXT_PIECES];
} bag_t;

void init_bag(bag_t *bag);

void generate_bag(bag_t *bag);

PIECE_ID peek_piece(bag_t *bag);

PIECE_ID grab_piece(bag_t *bag);

#endif
