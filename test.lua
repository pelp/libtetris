local ffi = require("ffi")

local C = ffi.load("./build/libtetris.so")

ffi.cdef[[
void *create_game();
void destroy_game(void* game);
int tick(void *game, void params);
char read_game(void *game, int x, int y);
void init(void *game,
          int width,
          int height,
          int fall_interval,
          int delayed_auto_shift,
          int automatic_repeat_rate);
]]

p_game = C.create_game()
C.destroy_game(p_game)
