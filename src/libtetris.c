#include "libtetris.h"
#include <inttypes.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <stdio.h>

int tetris_step(tetris_t *game);

int rotate(tetris_t *game, const rotation_t amount);

int rotate_cw(tetris_t *game);

int rotate_ccw(tetris_t *game);

int shift(tetris_t *game, const coord_t dx);

int left(tetris_t *game);

int right(tetris_t *game);

void ghost(tetris_t *game);

void grab_next_piece(tetris_t *game);

bool is_touching(tetris_t *game, const coord_t dx, const coord_t dy, const rotation_t rotation);

bool is_on_ground(tetris_t *game);

bool can_reset_lock_delay(tetris_t *game);

void reset_lock_delay(tetris_t *game);

bool increment_hold(tetris_hold_time_t *hold, tetris_inputs_t inputs, time_us_t delta_time);

tetris_input_state_t get_keys(tetris_t *game, tetris_params_t params);

void reset_piece_position(tetris_t *game);


#ifdef RECORD_TRANSACTIONS
void set_transaction_heap_size(transaction_list_t *list, uint64_t heap_size)
{
    list->heap_size = heap_size;
    if (list->transactions == NULL)
    {
        list->transactions = malloc(list->heap_size * sizeof(tetris_transaction_t));
    }
    else
    {
        list->transactions = realloc(list->transactions, list->heap_size * sizeof(tetris_transaction_t));
    }
}

void init_transaction_list(transaction_list_t *list)
{
    list->used_size = 0;
    set_transaction_heap_size(list, TRANSACTION_LIST_INIT_SIZE);
}

void add_transaction(transaction_list_t *list, tetris_params_t params)
{
    if (list->used_size == list->heap_size)
    {
        // Increase heap size when needed
        set_transaction_heap_size(list, list->heap_size * 2);
    }
    list->transactions[list->used_size++] = (tetris_transaction_t){
        .params = params
    };
}

TETRIS_API transaction_list_t read_transactions(tetris_t *game)
{
    return game->transaction_list;
}
#endif

TETRIS_API void run_transactions(tetris_t *game, tetris_transaction_t *list, int length) {
    printf("transactions: %d\n", length);
    for (int i = 0; i < length; i++) {
        printf("ticking: %d (%"PRId64" us)\n", i, list[i].params.delta_time);
        tick(game, list[i].params);
    }
}

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

bool is_on_ground(tetris_t *game) {
    return game->y == game->ghosty;
}

bool can_reset_lock_delay(tetris_t *game) {
    return game->lock_resets > 0;
}

void reset_lock_delay(tetris_t *game) {
    --game->lock_resets;
    game->lock_time = game->lock_delay;
    printf("Reset lock delay: %d\n", game->lock_resets);
}

int shift(tetris_t *game, const coord_t dx) {
    const bool on_ground_before_shift = is_on_ground(game);
    if (!is_touching(game, dx, 0, game->rotation)) {
        game->x += dx;
        if (on_ground_before_shift && can_reset_lock_delay(game)) {
            reset_lock_delay(game);
        }
        ghost(game);
        return 0;
    }
    return 1;
}

int left(tetris_t *game) {
    return shift(game, -1);
}

int right(tetris_t *game) {
    return shift(game, 1);
}

int rotate(tetris_t *game, const rotation_t amount) {
    const rotation_t new_rotation = (game->rotation + amount) % 4;
    const bool on_ground_before_rotation = is_on_ground(game);
    if (!is_touching(game, 0, 0, new_rotation)) {
        game->rotation = new_rotation;
        if (on_ground_before_rotation && can_reset_lock_delay(game)) {
            reset_lock_delay(game);
        }
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
                if (on_ground_before_rotation && can_reset_lock_delay(game)) {
                    reset_lock_delay(game);
                }
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
    game->lock_delay = 500000;
    game->lock_time = game->lock_delay;
    game->lock_resets = 15;
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

int tetris_step(tetris_t *game) {
    ghost(game);
    // Check if touching ground
    if (is_on_ground(game)) {
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
                for (int k = i; k >= 0; k--) {
                    for (int j = 0; j < game->framebuffer.width; j++) {
                        game->framebuffer.blocks[k * game->framebuffer.width + j] =
                                k == 0
                                ? PIECE_EMPTY
                                : game->framebuffer.blocks[(k - 1) * game->framebuffer.width + j];
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


bool increment_hold(tetris_hold_time_t *hold, tetris_inputs_t inputs, time_us_t delta_time) {
    bool update = false;
    for (size_t i = 0; i < sizeof(tetris_inputs_t) / sizeof(bool); i++) {
        if (((bool *) &inputs)[i]) {
            ((time_us_t *) hold)[i] += delta_time;
        } else {
            if (((time_us_t *) hold)[i] == 0) continue;
            ((time_us_t *) hold)[i] = 0;
        }
        update = true;
    }
    return update;
}

tetris_input_state_t get_keys(tetris_t *game, tetris_params_t params) {
    bool update = increment_hold(&game->input_time, game->input_state, params.delta_time);
    game->input_state = params.inputs;
    tetris_inputs_t edge;
    tetris_inputs_t hold;
    for (size_t i = 0; i < sizeof(tetris_inputs_t) / sizeof(bool); i++) {
        const int t = ((time_us_t *) &game->input_time)[i];
        const bool input = ((bool *) &game->input_state)[i];
        ((bool *) &hold)[i] = t > game->delayed_auto_shift;
        ((bool *) &edge)[i] = t == 0 && input;
        if (t > game->delayed_auto_shift) ((time_us_t *) &game->input_time)[i] -= game->automatic_repeat_rate;
    }
    return (tetris_input_state_t) {
            .hold = hold,
            .edge = edge,
            .update = update
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
    if (game->seed == 0) game->seed = (seed_t) time(NULL);
    srand48(game->seed);

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

#ifdef RECORD_TRANSACTIONS
    game->last_change = 0;
    init_transaction_list(&game->transaction_list);
#endif
    memset(&game->input_state, 0, sizeof(tetris_inputs_t));

    reset_piece_position(game);
}

TETRIS_API int tick(tetris_t *game, tetris_params_t params) {
    // Input hold management
    int rc = -1;
    tetris_input_state_t state = get_keys(game, params);
    int pre_tick_lines = game->lines;

    // Input logic
    if (state.edge.hard_drop) {
        // Fall all the way down
        while ((rc = tetris_step(game)) == 0);
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
    if (is_on_ground(game)) {
        game->lock_time -= params.delta_time;
        if(game->lock_time <= 0){
            rc = tetris_step(game);
        }else{
            rc = 5;
        }
    } else {
        const time_us_t drop_factor = params.inputs.soft_drop ? 6 : 1;
        game->fall_time -= params.delta_time * drop_factor;
        while (game->fall_time <= 0) {
            game->fall_time += game->fall_interval;
            rc = tetris_step(game);
        }
    }

    if (game->lines > pre_tick_lines + 3) rc = 3;

#ifdef RECORD_TRANSACTIONS
    game->last_change += params.delta_time;
#endif
    if (rc == -1 && state.update) rc = 4;
    if (rc != -1 || params.inputs.soft_drop) {
#ifdef RECORD_TRANSACTIONS
        // Save transaction of the tick if something happened
        add_transaction(&game->transaction_list, (tetris_params_t){
            .inputs = params.inputs,
            .delta_time = game->last_change
        });
        printf("recorded transaction: %ld, %d\n", game->last_change, rc);
        game->last_change = 0;
#endif
        // Set new fall interval
        game->fall_interval -= 10000 * (game->lines - pre_tick_lines);
    }

    return rc;
}

TETRIS_API int8_t read_game(tetris_t *game, coord_t x, coord_t y) {
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

TETRIS_API int get_lines(tetris_t *game) {
    return game->lines;
}

TETRIS_API void set_seed(tetris_t *game, seed_t seed) {
    game->seed = seed;
}