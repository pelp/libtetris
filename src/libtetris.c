#include "libtetris.h"
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <stdio.h>

TETRIS_API void *create_game(){
    printf("Creating game...\n");
    void* data = malloc(sizeof(tetris_t));
    memset(data, 0, sizeof(tetris_t));
    return data;
}
TETRIS_API void destroy_game(void* game){
    printf("Destroying game...\n");
    free(game);
}

int step(tetris_t *game);
int rotate(tetris_t *game, char amount);
int rotate_cw(tetris_t *game);
int rotate_ccw(tetris_t *game);
int left(tetris_t *game);
int right(tetris_t *game);
void ghost(tetris_t *game);
void place_piece(tetris_t *game);
void generate_bag(bag_t *bag);
void init_bag(bag_t *bag);
piece_t *grab_piece(bag_t *bag);
int tile_coord_rotate(tetris_t *game, int x, int y);
int touching(tetris_t *game, int dx, int dy);
int set_rotation(tetris_t *game, int rotation);
void increment_hold(tetris_t * game, tetris_params_t params);
tetris_input_state_t get_keys(tetris_t * game, tetris_params_t params);
void reset_piece_position(tetris_t *game);

void init_bag(bag_t *bag)
{
    generate_bag(bag);
    for(int i = 0; i < NUM_NEXT_PIECES; ++i)
    {
        bag->next[i] = bag->order[i];
        ++bag->current;
    }
}
void generate_bag(bag_t *bag)
{
    bag->current = 0;
    for (int i = 0; i < NUM_PIECES; ++i) bag->order[i] = i;
    for (int i = NUM_PIECES - 1; i > 0; --i)
    {
        int j = rand() % (i+1);
        int temp = bag->order[i];
        bag->order[i] = bag->order[j];
        bag->order[j] = temp;
    }
}

piece_t *peek_piece(bag_t *bag)
{
    return pieces[bag->order[bag->current]];
}

piece_t *grab_piece(bag_t *bag)
{
    // Take first piece in queue
    piece_t *piece = pieces[bag->next[0]];

    // Move the rest of the pieces one step
    for(int i = 0; i < NUM_NEXT_PIECES-1; ++i)
    {
        bag->next[i] = bag->next[i+1];
    }

    // Take a piece from the bag and put it at the end of the queue
    bag->next[NUM_NEXT_PIECES-1] = bag->order[bag->current];

    // Update the bag
    if(++bag->current >= NUM_PIECES) generate_bag(bag);

    return piece;
}

int tile_coord_rotate(tetris_t *game, int x, int y)
{
     switch(game->rotation)
     {
        case 0:
            return game->rotated.width * y + x;
        case 1:
            return game->rotated.height * (game->rotated.width - x - 1) + y;
        case 2:
            return game->rotated.width * (game->rotated.height - y) - x - 1;
        case 3:
            return game->rotated.height * x - y + game->rotated.height - 1;
        default:
            return 0;
     }
}

int touching(tetris_t *game, int dx, int dy)
{
    int ox = game->x + game->ox + dx;
    int oy = game->y + game->oy + dy;
    for (int i = 0; i < game->rotated.height; i++)
    {
        for (int j = 0; j < game->rotated.width; j++)
        {
            int piece_coord = tile_coord_rotate(game, j, i);
            if (game->current->tiles[piece_coord] == 0) continue;
            int game_coord = (i + oy) * game->width + // y
                             j + ox; // x
            if ((i + oy >= game->height) ||
                (j + ox >= game->width) ||
                (j + ox < 0) ||
                (game->tiles[game_coord] > 0))
            {
                return 0;
            }
        }
    }
    return 1;
}

int left(tetris_t *game)
{
    if (touching(game, -1, 0))
    {
        game->x--;
        ghost(game);
        return 0;
    }
    return 1;
}

int right(tetris_t *game)
{
    if (touching(game, 1, 0))
    {
        game->x++;
        ghost(game);
        return 0;
    }
    return 1;
}

int set_rotation(tetris_t *game, int rotation)
{
    int old = game->rotation;
    game->rotation = rotation & 0xF;
    switch (game->rotation)
    {
        case 0:
        case 2:
            game->rotated.width = game->current->width;
            game->rotated.height = game->current->height;
            game->ox = -(game->rotated.width + 1) / 2;
            game->oy = -(game->rotated.height + 1) / 2;
            break;
        case 1:
        case 3:
            game->rotated.width = game->current->height;
            game->rotated.height = game->current->width;
            game->ox = -(game->rotated.width + 1) / 2;
            game->oy = -(game->rotated.height + 1) / 2;
            break;
    }
    if (rotation & 0xF0) return 1;
    if (touching(game, 0, 0) == 0)
    {
        set_rotation(game, old | 0xF0);
    }
    return 0;
}

int rotate(tetris_t *game, char amount)
{
    int rot = set_rotation(game, (game->rotation + amount) % 4);
    if (rot == 0) {
        ghost(game);
    }
    return rot;
}
int rotate_cw(tetris_t *game){
    return rotate(game, 1);
}
int rotate_ccw(tetris_t *game){
    return rotate(game, 3);
}
int hold(tetris_t *game){
    if (!game->can_hold) return 0;
    game->can_hold = false;
    if (game->hold){
        piece_t *held_piece = game->hold;
        game->hold = game->current;
        game->current = held_piece;
        reset_piece_position(game);
    }else{
        game->hold = game->current;
        place_piece(game);
    }
    return 1;
}
void place_piece(tetris_t *game)
{
    game->current = grab_piece(&game->bag);
    reset_piece_position(game);
}
void reset_piece_position(tetris_t *game)
{
    game->y = 0;
    game->x = game->width / 2;
    set_rotation(game, 0);
    ghost(game);
}


TETRIS_API void init(
    tetris_t *game,
    int width,
    int height,
    time_us_t fall_interval,
    time_us_t delayed_auto_shift,
    time_us_t automatic_repeat_rate
)
{
    // Seed random generator with time
    srand((unsigned) time(NULL));

    game->width = width;
    game->height = height;

    size_t tiles_bytes = sizeof(char) * width * height;
    if (game->tiles == NULL)
    {
        game->tiles = malloc(tiles_bytes);
    }
    else
    {
        game->tiles = realloc(game->tiles, tiles_bytes);
    }
    memset(game->tiles, 0, tiles_bytes);

    game->rotated.width = 0;
    game->rotated.height = 0;

    init_bag(&game->bag);
    game->current = grab_piece(&game->bag);
    game->hold = NULL;
    game->can_hold = true;

    game->x = game->width / 2;
    game->y = 0;
    game->lines = 0;
    for (size_t i = 0; i < sizeof(tetris_inputs_t) / sizeof(bool); i++)
    {
        ((time_us_t*)&game->input_time)[i * sizeof(bool)] = 0;
    }
    game->fall_time = game->fall_interval = fall_interval;
    game->delayed_auto_shift = delayed_auto_shift;
    game->automatic_repeat_rate = automatic_repeat_rate;
    set_rotation(game, 0);
    ghost(game);
}

int solidify(tetris_t *game)
{
    for (int i = 0; i < game->rotated.height; i++)
    {
        for (int j = 0; j < game->rotated.width; j++)
        {
            int piece_coord = tile_coord_rotate(game, j, i);
            int game_coord = (i + game->y + game->oy) * game->width + j + game->x + game->ox;
            if (game_coord < 0) return 1;
            if (game->tiles[game_coord] == 0)
            {
                game->tiles[game_coord] = game->current->tiles[piece_coord];
            }
        }
    }
    return 0;
}

void ghost(tetris_t *game)
{
    int oldy = game->y;
    while(touching(game, 0, 1) != 0)
    {
        game->y++;
    }
    int resy = game->y;
    game->ghosty = resy;
    game->y = oldy;
}

int step(tetris_t *game)
{
    ghost(game);
    // Check if touching pieces
    if (game->ghosty - game->y == 0)
    {
        if (solidify(game))
        {
            // Lost game
            return 1;
        }
        // Check rows
        for (int i = 0; i < game->height; i++)
        {
            int count = 0;
            for (int j = 0; j < game->width; j++)
            {
                if (game->tiles[i * game->width + j] == 0) break;
                count++;
            }
            if (count == game->width)
            {
                game->lines++;
                // Move everything down
                for (int k = i; k > 0; k--)
                {
                    for (int j = 0; j < game->width; j++)
                    {
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

TETRIS_API char read_game(tetris_t *game, int x, int y)
{
    int piece_coord_y = y - game->y - game->oy;
    int ghost_coord_y = y - game->ghosty - game->oy;
    int piece_coord_x = x - game->x - game->ox;
    int piece_coord = tile_coord_rotate(game, piece_coord_x, piece_coord_y);
    int ghost_coord = tile_coord_rotate(game, piece_coord_x, ghost_coord_y);
    if (piece_coord_x >= 0 && piece_coord_x < game->rotated.width &&
        piece_coord_y >= 0 && piece_coord_y < game->rotated.height &&
        game->current->tiles[piece_coord] > 0)
    {
        return game->current->tiles[piece_coord];
    }

    if (piece_coord_x >= 0 && piece_coord_x < game->rotated.width &&
        ghost_coord_y >= 0 && ghost_coord_y < game->rotated.height &&
        game->current->tiles[ghost_coord] > 0)
    {
        return game->current->tiles[ghost_coord] > 0 ? 10 : 0;
    }

    if (piece_coord_x >= 0 && piece_coord_x < game->rotated.width &&
        piece_coord_y >= 0 && piece_coord_y < game->rotated.height &&
        game->current->tiles[piece_coord] > 0)
    {
        return game->current->tiles[piece_coord];
    }
    else
    {
        return game->tiles[y * game->width + x];
    }
}

void increment_hold(tetris_t * game, tetris_params_t params)
{
    for (size_t i = 0; i < sizeof(tetris_inputs_t) / sizeof(bool); i++)
    {
        if (((bool*)&params.inputs)[i * sizeof(bool)])
        {
            ((time_us_t*)&game->input_time)[i * sizeof(bool)] += params.delta_time;
        }
        else
        {
            ((time_us_t*)&game->input_time)[i * sizeof(bool)] = 0;
        }
    }
}

tetris_input_state_t get_keys(tetris_t * game, tetris_params_t params)
{
    tetris_hold_time_t old_hold = game->input_time;
    increment_hold(game, params);
    tetris_inputs_t edge;
    tetris_inputs_t hold;
    for (size_t i = 0; i < sizeof(tetris_inputs_t) / sizeof(bool); i++)
    {
        const int t = ((time_us_t*)&game->input_time)[i * sizeof(bool)];
        const int old_t = ((time_us_t*)&old_hold)[i * sizeof(bool)];
        ((bool*)&hold)[i * sizeof(bool)] = (
            t > game->delayed_auto_shift
        );
        ((bool*)&edge)[i * sizeof(bool)] = (
            t > 0 && old_t == 0
        );
        if (t > game->delayed_auto_shift) ((time_us_t*)&game->input_time)[i * sizeof(bool)] -= game->automatic_repeat_rate;
    }
    return (tetris_input_state_t){
        .hold = hold,
        .edge = edge
    };
}

TETRIS_API int tick(tetris_t *game, tetris_params_t params)
{
    // Input hold management
    int rc = -1;
    tetris_input_state_t state = get_keys(game, params);

    // Input logic
    if (state.hold.down || state.edge.down)
    {
        rc = step(game);
    }
    if (state.edge.space)
    {
        // Fall all the way down
        while ((rc = step(game)) == 0);
    }
    if (state.hold.left || state.edge.left)
    {
        rc = 0;
        left(game);
    }
    if (state.hold.right || state.edge.right)
    {
        rc = 0;
        right(game);
    }
    if (state.edge.rotate_ccw)
    {
        rc = 0;
        rotate_ccw(game);
    }
    if (state.edge.rotate_cw)
    {
        rc = 0;
        rotate_cw(game);
    }
    if (state.edge.hold)
    {
        rc = 0;
        hold(game);
    }

    // Force fall on fall interval
    game->fall_time -= params.delta_time;
    while (game->fall_time <= 0)
    {
        game->fall_time += game->fall_interval;
        rc = step(game);
    }
    return rc;
}