#include <stdbool.h>
#define NUM_PIECES 7

typedef struct
{
    int width, height;
    char tiles[];
} piece_t;

typedef struct
{
    int width;
    int height;
} translation_t;

typedef struct
{
    int current;
    int order[NUM_PIECES];
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
    int rotate_cw;
    int rotate_ccw;
    int hold;
    int down;
    int left;
    int right;
    int space;
} tetris_hold_time_t;

typedef struct
{
    tetris_inputs_t inputs;
    int delta_time_us;
} tetris_params_t;

typedef struct
{
    int width, height;
    char *tiles;
    int x, y;
    int ghosty;
    int ox, oy;
    piece_t *current;
    piece_t *next;
    bag_t bag;
    translation_t rotated;
    int rotation;
    int lines;
    int tick_time_us;
    int tick_interval_us;
    int tick;
    int fall_interval_ticks;
    int hold_delay;
    int hold_interval;
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

int step(tetris_t *game);
int rotate(tetris_t *game, char amount);
int rotate_cw(tetris_t *game);
int rotate_ccw(tetris_t *game);
int left(tetris_t *game);
int right(tetris_t *game);
void ghost(tetris_t *game);
void place_piece(tetris_t *game);
void generate_bag(bag_t *bag);
piece_t *peek_piece(bag_t *bag);
piece_t *grab_piece(bag_t *bag);
int tile_coord_rotate(tetris_t *game, int x, int y);
int touching(tetris_t *game, int dx, int dy);
int set_rotation(tetris_t *game, int rotation);

// Public API
int tick(tetris_t *game, tetris_params_t params);
char read_game(tetris_t *game, int x, int y);
void init(tetris_t *game, int width, int height, int tick_interval_us, int fall_interval_ticks, int hold_delay_ticks, int hold_interval_ticks);