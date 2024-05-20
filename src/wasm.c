#include "libtetris.h"
#include <stdio.h>
#include <time.h>
#include <emscripten/emscripten.h>

EMSCRIPTEN_KEEPALIVE tetris_t game;
EMSCRIPTEN_KEEPALIVE char *interface = NULL;

EMSCRIPTEN_KEEPALIVE void js_init(
    int width,
    int height,
    time_us_t fall_interval,
    time_us_t delayed_auto_shift,
    time_us_t automatic_repeat_rate
)
{
    init(&game, width, height, fall_interval, delayed_auto_shift, automatic_repeat_rate);
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

EMSCRIPTEN_KEEPALIVE int js_hold_width()
{
    return game.hold->width;
}

EMSCRIPTEN_KEEPALIVE int js_hold_height()
{
    return game.hold->height;
}

EMSCRIPTEN_KEEPALIVE char * js_hold()
{
    if (game.hold) return game.hold->tiles;
    return NULL;
}

EMSCRIPTEN_KEEPALIVE void js_set_fall_interval(time_us_t fall_interval)
{
    game.fall_interval = fall_interval;
}

EMSCRIPTEN_KEEPALIVE int js_tick(
        bool space,
        bool down,
        bool left,
        bool right,
        bool rotate_cw,
        bool rotate_ccw,
        bool hold,
        time_us_t delta_time
)
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
        .delta_time = delta_time
    });
}