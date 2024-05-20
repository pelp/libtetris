#include "libtetris.h"
#include <stdio.h>
#include <time.h>
#include <emscripten/emscripten.h>

EMSCRIPTEN_KEEPALIVE tetris_t game;
EMSCRIPTEN_KEEPALIVE char *interface = NULL;

EMSCRIPTEN_KEEPALIVE void js_init(
    int width,
    int height,
    int tick_interval_us,
    int fall_interval_ticks,
    int hold_delay_ticks,
    int hold_interval_ticks
)
{
    init(&game, width, height, tick_interval_us, fall_interval_ticks, hold_delay_ticks, hold_interval_ticks);
}

EMSCRIPTEN_KEEPALIVE int js_lines()
{
    return game.lines;
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

EMSCRIPTEN_KEEPALIVE int js_tick(bool space, bool down, bool left, bool right, bool rotate_cw, bool rotate_ccw, bool hold, int delta_time_us)
{
    return tick(&game, (tetris_params_t){
        .inputs = (tetris_inputs_t){
            .space = space,
            .down = down,
            .left = left,
            .right = right,
            .rotate_ccw = rotate_ccw,
            .rotate_cw = rotate_cw,
            .hold = hold
        },
        .delta_time_us = delta_time_us
    });
}