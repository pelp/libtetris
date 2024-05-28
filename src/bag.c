#include "bag.h"

void init_bag(bag_t *bag) {
    generate_bag(bag);
    for (int i = 0; i < NUM_NEXT_PIECES; ++i) {
        bag->next[i] = bag->order[i];
        ++bag->current;
    }
}

void generate_bag(bag_t *bag) {
    bag->current = 0;
    for (int i = 0; i < PIECE_COUNT; ++i) bag->order[i] = i;
    for (int i = PIECE_COUNT - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        int temp = bag->order[i];
        bag->order[i] = bag->order[j];
        bag->order[j] = temp;
    }
}

PIECE_ID peek_piece(bag_t *bag) {
    return pieces[bag->order[bag->current]];
}

PIECE_ID grab_piece(bag_t *bag) {
    // Take first piece in queue
    PIECE_ID piece = pieces[bag->next[0]];

    // Move the rest of the pieces one step
    for (int i = 0; i < NUM_NEXT_PIECES - 1; ++i) {
        bag->next[i] = bag->next[i + 1];
    }

    // Take a piece from the bag and put it at the end of the queue
    bag->next[NUM_NEXT_PIECES - 1] = bag->order[bag->current];

    // Update the bag
    if (++bag->current >= PIECE_COUNT) generate_bag(bag);

    return piece;
}
