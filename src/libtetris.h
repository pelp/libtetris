#include <stdbool.h>
#include <stdint.h>

#ifdef _WIN32
    #define TETRIS_API __declspec(dllexport)
#elif __GNUC__ >= 4
    #define TETRIS_API __attribute__ ((visibility ("default")))
#else
    #define TETRIS_API
#endif

// We assume the number of pieces in the bag to be greater than the number of next pieces to preview
#define NUM_PIECES 7
#define NUM_NEXT_PIECES 5

typedef int time_us_t;

typedef struct
{
    int width, height;
    char tiles[];
} piece_t;

typedef struct
{
    int width;
    int height;
} rotation_t;

typedef struct
{
    uint8_t current;
    uint8_t order[NUM_PIECES];
    uint8_t next[NUM_NEXT_PIECES];
} bag_t;

typedef struct
{
    bool rotate_cw;
    bool rotate_ccw;
    bool hold;
    bool down;
    bool left;
    bool right;
    bool space;
} tetris_inputs_t;

typedef struct
{
    tetris_inputs_t edge;
    tetris_inputs_t hold;
} tetris_input_state_t;

typedef struct
{
    time_us_t rotate_cw;
    time_us_t rotate_ccw;
    time_us_t hold;
    time_us_t down;
    time_us_t left;
    time_us_t right;
    time_us_t space;
} tetris_hold_time_t;

typedef struct
{
    tetris_inputs_t inputs;
    time_us_t delta_time;
} tetris_params_t;

typedef struct
{
    int width, height;
    char *tiles;
    int x, y;
    int ghosty;
    int ox, oy;
    piece_t *current;
    piece_t *hold;
    bool can_hold;
    bag_t bag;
    rotation_t rotated;
    int rotation;
    int lines;
    time_us_t fall_interval;
    time_us_t fall_time;
    time_us_t delayed_auto_shift;
    time_us_t automatic_repeat_rate;
    tetris_hold_time_t input_time;
} tetris_t;

static piece_t PIECE_I = {4, 4, {
        0, 0, 0, 0,
        2, 2, 2, 2,
        0, 0, 0, 0,
        0, 0, 0, 0}};
static piece_t PIECE_J = {3, 3, {
        3, 0, 0,
        3, 3, 3,
        0, 0, 0}};
static piece_t PIECE_L = {3, 3, {
        0, 0, 4,
        4, 4, 4,
        0, 0, 0}};
static piece_t PIECE_O = {2, 2, {
        1, 1,
        1, 1}};
static piece_t PIECE_S = {3, 3, {
        0, 6, 6,
        6, 6, 0,
        0, 0, 0}};
static piece_t PIECE_T = {3, 3, {
        0, 7, 0,
        7, 7, 7,
        0, 0, 0}};
static piece_t PIECE_Z = {3, 3, {
        5, 5, 0,
        0, 5, 5,
        0, 0, 0}};
static piece_t *pieces[] = {&PIECE_I, &PIECE_J, &PIECE_L, &PIECE_O, &PIECE_S, &PIECE_T, &PIECE_Z};

// Public API
TETRIS_API void *create_game();
TETRIS_API void destroy_game(void* game);
TETRIS_API int tick(tetris_t *game, tetris_params_t params);
TETRIS_API char read_game(tetris_t *game, int x, int y);
TETRIS_API void init(tetris_t *game,
          int width,
          int height,
          time_us_t fall_interval,
          time_us_t delayed_auto_shift,
          time_us_t automatic_repeat_rate);