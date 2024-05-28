#include "piece.h"

int get_piece_width(PIECE_ID id) {
    int min_x = 0, max_x = 0;
    for (int i = 0; i < 4; ++i) {
        int x = PIECE_DATA_X[id][0][i];
        min_x = min_x < x ? min_x : x;
        max_x = max_x > x ? max_x : x;
    }
    return max_x - min_x + 1;
}

int get_piece_height(PIECE_ID id) {
    int min_y = 0, max_y = 0;
    for (int i = 0; i < 4; ++i) {
        int y = PIECE_DATA_Y[id][0][i];
        min_y = min_y < y ? min_y : y;
        max_y = max_y > y ? max_y : y;
    }
    return max_y - min_y + 1;
}

coord_t *get_piece_x_coords(PIECE_ID id) {
    return PIECE_DATA_X[id][0];
}

coord_t *get_piece_y_coords(PIECE_ID id) {
    return PIECE_DATA_Y[id][0];
}
