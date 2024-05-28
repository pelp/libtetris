#include "libtetris.h"
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <stdio.h>

int step(tetris_t *game);

int rotate(tetris_t *game, rotation_t amount);

int rotate_cw(tetris_t *game);

int rotate_ccw(tetris_t *game);

int left(tetris_t *game);

int right(tetris_t *game);

void ghost(tetris_t *game);

void place_piece(tetris_t *game);

bool is_touching(tetris_t *game, coord_t dx, coord_t dy, rotation_t rotation);

void increment_hold(tetris_t *game, tetris_params_t params);

tetris_input_state_t get_keys(tetris_t *game, tetris_params_t params);

void reset_piece_position(tetris_t *game);

bool is_touching(tetris_t *game, coord_t dx, coord_t dy, rotation_t rotation) {
    const coord_t x = game->x + dx;
    const coord_t y = game->y + dy;

    for (int i = 0; i < BLOCK_COUNT; ++i) {
        const coord_t block_x = x + PIECE_DATA_X[game->current][rotation][i];
        const coord_t block_y = y + PIECE_DATA_Y[game->current][rotation][i];

        if (block_x < 0 || block_x >= game->width) return true;
        if (block_x >= game->height) return true;
        if (block_y < 0) continue;

        const coord_t block_index = block_y * game->width + block_x;

        if (game->tiles[block_index] != PIECE_EMPTY) return true;
    }
    return false;
}

int left(tetris_t *game) {
    if (!is_touching(game, -1, 0, game->rotation)) {
        game->x--;
        ghost(game);
        return 0;
    }
    return 1;
}

int right(tetris_t *game) {
    if (!is_touching(game, 1, 0, game->rotation)) {
        game->x++;
        ghost(game);
        return 0;
    }
    return 1;
}

int rotate(tetris_t *game, rotation_t amount) {
    rotation_t new_rotation = (game->rotation + amount) % 4;
    if (!is_touching(game, 0, 0, new_rotation)) {
        game->rotation = new_rotation;
        ghost(game);
        return 0;
    } else {
        kick_table_t *kick_table = game->current == PIECE_I ? &WALLKICK_I_CCW : &WALLKICK_NORMAL_CCW;
        kick_table_row_t *kick_table_row = &(*kick_table[game->rotation]);

        for (int i = 0; i < 4; ++i) {
            const coord_t dx = (*kick_table_row)[i][0];
            const coord_t dy = (*kick_table_row)[i][1];
            if (!is_touching(game, dx, dy, new_rotation)) {
                game->rotation = new_rotation;
                game->x += dx;
                game->y += dy;
                ghost(game);
                return 0;
            }
        }
    }
    return 1;
}

int rotate_cw(tetris_t *game) {
    return rotate(game, 1);
}

int rotate_ccw(tetris_t *game) {
    return rotate(game, 3);
}

int hold(tetris_t *game) {
    if (!game->can_hold) return 0;
    game->can_hold = false;
    if (game->hold) {
        PIECE_ID held_piece = game->hold;
        game->hold = game->current;
        game->current = held_piece;
        reset_piece_position(game);
    } else {
        game->hold = game->current;
        place_piece(game);
    }
    return 1;
}

void place_piece(tetris_t *game) {
    game->current = grab_piece(&game->bag);
    reset_piece_position(game);
}

void reset_piece_position(tetris_t *game) {
    game->y = 0;
    game->x = 3;
    game->rotation = 0;
    ghost(game);
}

int solidify(tetris_t *game) {
    for (int i = 0; i < BLOCK_COUNT; ++i) {
        const coord_t block_x = game->x + PIECE_DATA_X[game->current][game->rotation][i];
        const coord_t block_y = game->y + PIECE_DATA_Y[game->current][game->rotation][i];

        const coord_t block_index = block_y * game->width + block_x;

        if (block_index < 0) return 1;

        game->tiles[block_index] = game->current;
    }
    return 0;
}

void ghost(tetris_t *game) {
    game->ghosty = game->y;
    while (!is_touching(game, 0, game->ghosty - game->y + 1, game->rotation)) {
        ++game->ghosty;
    }
}

int step(tetris_t *game) {
    ghost(game);
    // Check if touching pieces
    if (game->ghosty - game->y == 0) {
        if (solidify(game)) {
            // Lost game
            return 1;
        }
        // Check rows
        for (int i = 0; i < game->height; i++) {
            int count = 0;
            for (int j = 0; j < game->width; j++) {
                if (game->tiles[i * game->width + j] == 0) break;
                count++;
            }
            if (count == game->width) {
                game->lines++;
                // Move everything down
                for (int k = i; k > 0; k--) {
                    for (int j = 0; j < game->width; j++) {
                        game->tiles[k * game->width + j] = game->tiles[(k - 1) * game->width + j];
                    }
                }

            }
        }
        // Next piece
        place_piece(game);
        game->can_hold = true;
        return 2;
    }

    // Nothing happens
    game->y++;
    return 0;
}


void increment_hold(tetris_t *game, tetris_params_t params) {
    for (size_t i = 0; i < sizeof(tetris_inputs_t) / sizeof(bool); i++) {
        if (((bool *) &params.inputs)[i * sizeof(bool)]) {
            ((time_us_t *) &game->input_time)[i * sizeof(bool)] += params.delta_time;
        } else {
            ((time_us_t *) &game->input_time)[i * sizeof(bool)] = 0;
        }
    }
}

tetris_input_state_t get_keys(tetris_t *game, tetris_params_t params) {
    tetris_hold_time_t old_hold = game->input_time;
    increment_hold(game, params);
    tetris_inputs_t edge;
    tetris_inputs_t hold;
    for (size_t i = 0; i < sizeof(tetris_inputs_t) / sizeof(bool); i++) {
        const int t = ((time_us_t *) &game->input_time)[i * sizeof(bool)];
        const int old_t = ((time_us_t *) &old_hold)[i * sizeof(bool)];
        ((bool *) &hold)[i * sizeof(bool)] = (
                t > game->delayed_auto_shift
        );
        ((bool *) &edge)[i * sizeof(bool)] = (
                t > 0 && old_t == 0
        );
        if (t > game->delayed_auto_shift)
            ((time_us_t *) &game->input_time)[i * sizeof(bool)] -= game->automatic_repeat_rate;
    }
    return (tetris_input_state_t) {
            .hold = hold,
            .edge = edge
    };
}


// Public API
TETRIS_API tetris_t *create_game() {
    tetris_t *data = (tetris_t *) malloc(sizeof(tetris_t));
    memset(data, 0, sizeof(tetris_t));
    return data;
}

TETRIS_API void destroy_game(tetris_t *game) {
    free(game);
}

TETRIS_API void init(
        tetris_t *game,
        int width,
        int height,
        time_us_t fall_interval,
        time_us_t delayed_auto_shift,
        time_us_t automatic_repeat_rate
) {
    // Seed random generator with time
    srand((unsigned) time(NULL));

    game->width = width;
    game->height = height;

    size_t tiles_bytes = sizeof(PIECE_ID) * width * height;
    if (game->tiles == NULL) {
        game->tiles = malloc(tiles_bytes);
    } else {
        game->tiles = realloc(game->tiles, tiles_bytes);
    }
    memset(game->tiles, PIECE_EMPTY, tiles_bytes);

    game->rotated.width = 0;
    game->rotated.height = 0;

    init_bag(&game->bag);
    game->current = grab_piece(&game->bag);
    game->hold = NULL;
    game->can_hold = true;

    game->x = game->width / 2;
    game->y = 0;
    game->lines = 0;
    for (size_t i = 0; i < sizeof(tetris_inputs_t) / sizeof(bool); i++) {
        ((time_us_t *) &game->input_time)[i * sizeof(bool)] = 0;
    }
    game->fall_time = game->fall_interval = fall_interval;
    game->delayed_auto_shift = delayed_auto_shift;
    game->automatic_repeat_rate = automatic_repeat_rate;
    set_rotation(game, 0);
    ghost(game);
}

TETRIS_API int tick(tetris_t *game, tetris_params_t params) {
    // Input hold management
    int rc = -1;
    tetris_input_state_t state = get_keys(game, params);
    int pre_tick_lines = game->lines;

    // Input logic
    if (state.hold.down || state.edge.down) {
        rc = step(game);
    }
    if (state.edge.space) {
        // Fall all the way down
        while ((rc = step(game)) == 0);
    }
    if (state.hold.left || state.edge.left) {
        rc = 0;
        left(game);
    }
    if (state.hold.right || state.edge.right) {
        rc = 0;
        right(game);
    }
    if (state.edge.rotate_ccw) {
        rc = 0;
        rotate_ccw(game);
    }
    if (state.edge.rotate_cw) {
        rc = 0;
        rotate_cw(game);
    }
    if (state.edge.hold) {
        rc = 0;
        hold(game);
    }

    // Force fall on fall interval
    game->fall_time -= params.delta_time;
    while (game->fall_time <= 0) {
        game->fall_time += game->fall_interval;
        rc = step(game);
    }
    if (game->lines > pre_tick_lines + 3) rc = 3;
    return rc;
}

TETRIS_API char read_game(tetris_t *game, int x, int y) {
    int piece_coord_y = y - game->y - game->oy;
    int ghost_coord_y = y - game->ghosty - game->oy;
    int piece_coord_x = x - game->x - game->ox;
    int piece_coord = tile_coord_rotate(game, piece_coord_x, piece_coord_y);
    int ghost_coord = tile_coord_rotate(game, piece_coord_x, ghost_coord_y);
    if (piece_coord_x >= 0 && piece_coord_x < game->rotated.width &&
        piece_coord_y >= 0 && piece_coord_y < game->rotated.height &&
        game->current->tiles[piece_coord] > 0) {
        return game->current->tiles[piece_coord];
    }

    if (piece_coord_x >= 0 && piece_coord_x < game->rotated.width &&
        ghost_coord_y >= 0 && ghost_coord_y < game->rotated.height &&
        game->current->tiles[ghost_coord] > 0) {
        return game->current->tiles[ghost_coord] > 0 ? 10 : 0;
    }

    if (piece_coord_x >= 0 && piece_coord_x < game->rotated.width &&
        piece_coord_y >= 0 && piece_coord_y < game->rotated.height &&
        game->current->tiles[piece_coord] > 0) {
        return game->current->tiles[piece_coord];
    } else {
        return game->tiles[y * game->width + x];
    }
}
