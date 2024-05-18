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
} tetris_t;

static piece_t PIECE_O = { 2, 2, { 1, 1, 1, 1 } };
static piece_t PIECE_I = { 1, 4, { 2, 2, 2, 2 } };
static piece_t PIECE_J = { 3, 2, { 3, 0, 0, 3, 3, 3 } };
static piece_t PIECE_L = { 3, 2, { 0, 0, 4, 4, 4, 4 } };
static piece_t PIECE_Z = { 3, 2, { 5, 5, 0, 0, 5, 5 } };
static piece_t PIECE_S = { 3, 2, { 0, 6, 6, 6, 6, 0 } };
static piece_t PIECE_T = { 3, 2, { 0, 7, 0, 7, 7, 7 } };
static piece_t *pieces[] = { &PIECE_O,
                             &PIECE_I,
                             &PIECE_J,
                             &PIECE_L,
                             &PIECE_Z,
                             &PIECE_S,
                             &PIECE_T };

void init(tetris_t *game, int width, int height);
int step(tetris_t *game);
int rotate(tetris_t *game);
int left(tetris_t *game);
int right(tetris_t *game);
char read_game(tetris_t *game, int x, int y);
