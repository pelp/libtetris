local ffi = require("ffi")

local M = {}
M.api = ffi.load("./.build/Debug/tetris")

ffi.cdef[[
typedef int time_us_t;
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
    tetris_inputs_t inputs;
    time_us_t delta_time;
} tetris_params_t;

void *create_game();
void destroy_game(void* game);
int tick(void *game, tetris_params_t params);
char read_game(void *game, int x, int y);
void init(void *game,
          int width,
          int height,
          time_us_t fall_interval,
          time_us_t delayed_auto_shift,
          time_us_t automatic_repeat_rate);
]]

M.tetris_inputs = ffi.metatype("tetris_inputs_t", {})
M.tetris_params = ffi.metatype("tetris_params_t", {})

return M