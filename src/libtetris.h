#ifndef LIBTETRIS_LIBTETRIS_H
#define LIBTETRIS_LIBTETRIS_H

#include "bag.h"
#include "framebuffer.h"

#ifdef _WIN32
#define TETRIS_API __declspec(dllexport)
#elif __GNUC__ >= 4
#define TETRIS_API __attribute__ ((visibility ("default")))
#else
#define TETRIS_API
#endif

typedef int64_t time_us_t;

typedef struct {
    bool rotate_cw;
    bool rotate_ccw;
    bool hold;
    bool down;
    bool left;
    bool right;
    bool space;
} tetris_inputs_t;

typedef struct {
    tetris_inputs_t edge;
    tetris_inputs_t hold;
} tetris_input_state_t;

typedef struct {
    time_us_t rotate_cw;
    time_us_t rotate_ccw;
    time_us_t hold;
    time_us_t down;
    time_us_t left;
    time_us_t right;
    time_us_t space;
} tetris_hold_time_t;

typedef struct {
    tetris_inputs_t inputs;
    time_us_t delta_time;
} tetris_params_t;

typedef struct {
    framebuffer_t framebuffer;

    coord_t x, y;      // Current x and y position
    coord_t ghosty;    // Y-coordinate used to compare with y-coordinate above to check if piece hit bottom
    rotation_t rotation; // Rotation of current piece
    piece_id_t current; // Current piece on the board
    piece_id_t hold;    // Held piece
    bool can_hold;    // True if the user can hold the current piece
    bag_t bag;        // Bag used for generating the sequence of pieces

    // TODO: Add score system
    int32_t lines; // Amount of lines cleared

    // Timers
    time_us_t fall_interval;         // The interval at which the piece falls down 1 tile
    time_us_t fall_time;             // The accumulator that tracks the falling piece
    time_us_t delayed_auto_shift;    // DAS: The time it takes for the piece movement to start repeating after initial movement
    time_us_t automatic_repeat_rate; // AAR: The rate at which the piece repeats the movement
    tetris_hold_time_t input_time;   // Input timers that keep track of how long a key has been held
} tetris_t;

// Public API
TETRIS_API tetris_t *create_game();

TETRIS_API void destroy_game(tetris_t *game);

TETRIS_API void init(tetris_t *game,
                     int width,
                     int height,
                     time_us_t fall_interval,
                     time_us_t delayed_auto_shift,
                     time_us_t automatic_repeat_rate);

TETRIS_API int tick(tetris_t *game, tetris_params_t params);

TETRIS_API char read_game(tetris_t *game, coord_t x, coord_t y);

#endif