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
    int width, height;
    char *tiles;
    int x, y;
    int ghosty;
    int ox, oy;
    piece_t *current;
    piece_t *next;
    translation_t rotated;
    int rotation;
    int lines;
} tetris_t;

static piece_t BOX = { 2, 2, { 1, 1, 1, 1 } };
static piece_t STRAIGHT = { 1, 4, { 2, 2, 2, 2 } };
static piece_t LEFT_L = { 3, 2, { 3, 0, 0, 3, 3, 3 } };
static piece_t RIGHT_L = { 3, 2, { 0, 0, 4, 4, 4, 4 } };
static piece_t LEFT_ZAG = { 3, 2, { 5, 5, 0, 0, 5, 5 } };
static piece_t RIGHT_ZAG = { 3, 2, { 0, 6, 6, 6, 6, 0 } };
static piece_t T = { 3, 2, { 0, 7, 0, 7, 7, 7 } };
static piece_t *pieces[] = { &BOX,
                             &STRAIGHT,
                             &LEFT_L,
                             &RIGHT_L,
                             &LEFT_ZAG,
                             &RIGHT_ZAG,
                             &T };

void init(tetris_t *game, int width, int height);
int step(tetris_t *game);
int rotate(tetris_t *game);
int left(tetris_t *game);
int right(tetris_t *game);
char read_game(tetris_t *game, int x, int y);
