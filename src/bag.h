#ifndef LIBTETRIS_BAG_H
#define LIBTETRIS_BAG_H

#include "piece.h"

#define NUM_NEXT_PIECES 5

typedef struct {
    piece_id_t current;
    piece_id_t order[PIECE_COUNT];
    piece_id_t next[NUM_NEXT_PIECES];
} bag_t;

void init_bag(bag_t *bag);

void generate_bag(bag_t *bag);

piece_id_t peek_piece(bag_t *bag);

piece_id_t grab_piece(bag_t *bag);

#endif
