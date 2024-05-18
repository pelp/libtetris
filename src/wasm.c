#include "libtetris.h"
#include <stdio.h>
#include <time.h>
#include <emscripten/emscripten.h>

EMSCRIPTEN_KEEPALIVE tetris_t game;
EMSCRIPTEN_KEEPALIVE char *interface = NULL;

EMSCRIPTEN_KEEPALIVE void js_init(int width, int height)
{
    init(&game, width, height);
}

EMSCRIPTEN_KEEPALIVE int js_lines()
{
    return game.lines;
}

EMSCRIPTEN_KEEPALIVE int js_step()
{
    return step(&game);
}

EMSCRIPTEN_KEEPALIVE int js_left()
{
    return left(&game);
}

EMSCRIPTEN_KEEPALIVE int js_right()
{
    return right(&game);
}

EMSCRIPTEN_KEEPALIVE int js_rotate()
{
    return rotate(&game);
}

EMSCRIPTEN_KEEPALIVE char * js_get()
{
    // Allocate interface buffer
    if (interface == NULL)
    {
        interface = malloc(sizeof(char) * game.width * game.height);
    }

    // Filter the data
    for (int i = 0; i < game.height; i++)
    {
        for (int j = 0; j < game.width; j++)
        {
            char state = read_game(&game, j, i);
            interface[i * game.width + j] = state;
        }
    }
    return interface;
}

EMSCRIPTEN_KEEPALIVE int js_next_width()
{
    return game.next->width;
}

EMSCRIPTEN_KEEPALIVE int js_next_height()
{
    return game.next->height;
}

EMSCRIPTEN_KEEPALIVE char * js_next()
{
    return game.next->tiles;
}
