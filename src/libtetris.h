#ifndef LIBTETRIS_LIBTETRIS_H
#define LIBTETRIS_LIBTETRIS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

#ifdef _WIN32
#define TETRIS_API __declspec(dllexport)
#elif __GNUC__ >= 4
#define TETRIS_API __attribute__((visibility("default")))
#else
#define TETRIS_API
#endif

typedef int64_t      time_us_t;
typedef unsigned int seed_t;

typedef struct {
    bool left;
    bool right;
    bool rotate_cw;
    bool rotate_ccw;
    bool soft_drop;
    bool hard_drop;
    bool hold;
} tetris_inputs_t;

typedef struct {
    tetris_inputs_t inputs;
    time_us_t       delta_time;
} tetris_params_t;

typedef struct {
    tetris_params_t params;
} tetris_transaction_t;

typedef struct {
    tetris_transaction_t *transactions;
    uint64_t              heap_size;
    uint64_t              used_size;
} transaction_list_t;

// Public Types
typedef struct tetris_input_state_t tetris_input_state_t;
typedef struct tetris_hold_time_t   tetris_hold_time_t;
typedef struct tetris_t             tetris_t;

// Public API
TETRIS_API tetris_t *create_game();

TETRIS_API void destroy_game(tetris_t *game);

TETRIS_API void init(tetris_t *game,
                     int       width,
                     int       height,
                     time_us_t fall_interval,
                     time_us_t delayed_auto_shift,
                     time_us_t automatic_repeat_rate);

TETRIS_API int tick(tetris_t *game, tetris_params_t params);

TETRIS_API int8_t read_game(tetris_t *game, coord_t x, coord_t y);

TETRIS_API transaction_list_t read_transactions(tetris_t *game);

TETRIS_API int get_lines(tetris_t *game);

TETRIS_API void set_seed(tetris_t *game, seed_t seed);

TETRIS_API uint8_t get_hold_width(tetris_t *game);
TETRIS_API uint8_t get_hold_height(tetris_t *game);
TETRIS_API const int8_t* get_hold_blocks(tetris_t *game);
TETRIS_API uint8_t get_next_width(tetris_t *game, int index);
TETRIS_API uint8_t get_next_height(tetris_t *game, int index);
TETRIS_API const int8_t* get_next_blocks(tetris_t *game, int index);

#ifdef __cplusplus
}
#endif

#endif
