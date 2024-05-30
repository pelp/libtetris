#include "libtetris.h"
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <stdio.h>

int step(tetris_t *game);

int rotate(tetris_t *game, const rotation_t amount);

int rotate_cw(tetris_t *game);

int rotate_ccw(tetris_t *game);

int left(tetris_t *game);

int right(tetris_t *game);

void ghost(tetris_t *game);

void grab_next_piece(tetris_t *game);

bool is_touching(tetris_t *game, const coord_t dx, const coord_t dy, const rotation_t rotation);

void increment_hold(tetris_t *game, tetris_params_t params);

tetris_input_state_t get_keys(tetris_t *game, tetris_params_t params);

void reset_piece_position(tetris_t *game);

bool is_touching(tetris_t *game, const coord_t dx, const coord_t dy, const rotation_t rotation) {
    const coord_t x = game->x + dx;
    const coord_t y = game->y + dy;

    for (int i = 0; i < BLOCK_COUNT; ++i) {
        const coord_t block_x = x + PIECE_DATA_X[game->current][rotation][i];
        const coord_t block_y = y + PIECE_DATA_Y[game->current][rotation][i];

        if (block_x < 0 || block_x >= game->framebuffer.width) return true;
        if (block_y >= game->framebuffer.height) return true;
        if (block_y < 0) continue;

        const coord_t block_index = block_y * game->framebuffer.width + block_x;

        if (game->framebuffer.blocks[block_index] != PIECE_EMPTY) return true;
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

int rotate(tetris_t *game, const rotation_t amount) {
    const rotation_t new_rotation = (game->rotation + amount) % 4;
    if (!is_touching(game, 0, 0, new_rotation)) {
        game->rotation = new_rotation;
        ghost(game);
        return 0;
    } else {
        const kick_table_t *const kick_table = game->current == PIECE_I ? &WALLKICK_I_CCW : &WALLKICK_NORMAL_CCW;

        for (int i = 0; i < KICK_COUNT; ++i) {
            const coord_t dx = (*kick_table)[game->rotation][i][0];
            const coord_t dy = (*kick_table)[game->rotation][i][1];
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
    if (game->hold != PIECE_EMPTY) {
        const piece_id_t held_piece = game->hold;
        game->hold = game->current;
        game->current = held_piece;
        reset_piece_position(game);
    } else {
        game->hold = game->current;
        grab_next_piece(game);
    }
    return 1;
}

void grab_next_piece(tetris_t *game) {
    game->current = grab_piece(&game->bag);
    reset_piece_position(game);
}

void reset_piece_position(tetris_t *game) {
    game->y = -2;
    game->x = 3;
    game->rotation = 0;
    ghost(game);
}

int solidify(tetris_t *game) {
    for (int i = 0; i < BLOCK_COUNT; ++i) {
        const coord_t block_x = game->x + PIECE_DATA_X[game->current][game->rotation][i];
        const coord_t block_y = game->y + PIECE_DATA_Y[game->current][game->rotation][i];

        const coord_t block_index = block_y * game->framebuffer.width + block_x;

        if (block_index < 0) return 1;

        game->framebuffer.blocks[block_index] = game->current;
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
        for (int i = 0; i < game->framebuffer.height; ++i) {
            int count = 0;
            for (int j = 0; j < game->framebuffer.width; ++j) {
                if (game->framebuffer.blocks[i * game->framebuffer.width + j] == PIECE_EMPTY) break;
                ++count;
            }
            if (count == game->framebuffer.width) {
                game->lines++;
                // Move everything down
                for (int k = i; k > 0; k--) {
                    for (int j = 0; j < game->framebuffer.width; j++) {
                        game->framebuffer.blocks[k * game->framebuffer.width + j] =
                                game->framebuffer.blocks[(k - 1) * game->framebuffer.width + j];
                    }
                }
            }
        }
        // Next piece
        grab_next_piece(game);
        game->can_hold = true;
        return 2;
    }

    // Nothing happens
    ++game->y;
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

    init_framebuffer(&game->framebuffer, width, height);

    init_bag(&game->bag);
    game->current = grab_piece(&game->bag);
    game->hold = PIECE_EMPTY;
    game->can_hold = true;

    game->lines = 0;
    for (size_t i = 0; i < sizeof(tetris_inputs_t) / sizeof(bool); i++) {
        ((time_us_t *) &game->input_time)[i * sizeof(bool)] = 0;
    }
    game->fall_time = 0;
    game->fall_interval = fall_interval;
    game->delayed_auto_shift = delayed_auto_shift;
    game->automatic_repeat_rate = automatic_repeat_rate;

    reset_piece_position(game);
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

TETRIS_API char read_game(tetris_t *game, coord_t x, coord_t y) {
    for (int i = 0; i < BLOCK_COUNT; ++i) {
        const coord_t block_x = game->x + PIECE_DATA_X[game->current][game->rotation][i];
        const coord_t block_y = game->y + PIECE_DATA_Y[game->current][game->rotation][i];
        if (block_x == x && block_y == y) {
            return game->current;
        }
    }

    for (int i = 0; i < BLOCK_COUNT; ++i) {
        const coord_t block_x = game->x + PIECE_DATA_X[game->current][game->rotation][i];
        const coord_t ghost_y = game->ghosty + PIECE_DATA_Y[game->current][game->rotation][i];
        if (block_x == x && ghost_y == y) {
            return PIECE_GHOST;
        }
    }

    return game->framebuffer.blocks[y * game->framebuffer.width + x];
}
