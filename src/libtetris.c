#include "libtetris.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void ghost(tetris_t *game);

void place_piece(tetris_t *game);

piece_t *get_piece()
{
    return pieces[rand() % (sizeof(pieces) / sizeof(piece_t *))];
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
            int game_coord = (i + oy) * game->width + // y
                             j + ox; // x
            if ((i + oy >= game->height) ||
                (j + ox >= game->width) ||
                (j + ox < 0) ||
                (game->tiles[game_coord] > 0 && game->current->tiles[piece_coord] > 0))
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
            game->ox = -game->rotated.width / 2;
            game->oy = -game->rotated.height / 2;
            break;
        case 1:
        case 3:
            game->rotated.width = game->current->height;
            game->rotated.height = game->current->width;
            game->ox = -game->rotated.width / 2;
            game->oy = -game->rotated.height / 2;
            break;
    }
    if (rotation & 0xF0) return 1;
    if (touching(game, 0, 0) == 0)
    {
        set_rotation(game, old | 0xF0);
    }
    return 0;
}

int rotate(tetris_t *game)
{
    int rot = set_rotation(game, (game->rotation + 1) % 4);
    if (rot == 0) {
        ghost(game);
    }
    return rot;
}

void init(tetris_t *game, int width, int height)
{
    // Seed random generator with time
    srand((unsigned) time(NULL));

    game->width = width;
    game->height = height;
    if (game->tiles == NULL)
    {
        game->tiles = malloc(sizeof(char) * width * height);
    }
    else
    {
        game->tiles = realloc(game->tiles, sizeof(char) * width * height);
    }
    for (int i = 0; i < game->width * game->height; i++)
    {
        game->tiles[i] = 0;
    }
    game->rotated.width = 0;
    game->rotated.height = 0;
    game->current = get_piece();
    game->next = get_piece();
    game->x = game->width / 2;
    game->y = 0;
    game->lines = 0;
    set_rotation(game, 0);
    ghost(game);
}

void place_piece(tetris_t *game)
{
    game->y = 0;
    game->x = game->width / 2;
    piece_t *n = get_piece();
    // Reroll to reduce duplicates
    if (n == game->next)
    {
        n = get_piece();
    }
    game->current = game->next;
    game->next = n;
    set_rotation(game, 0);
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
        ghost(game);
        return 2;
    }

    // Nothing happens
    game->y++;
    return 0;
}

char read_game(tetris_t *game, int x, int y)
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
