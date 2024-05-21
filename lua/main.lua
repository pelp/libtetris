local libtetris = require("libtetris")

tetris_game = nil

colors = {
  [1] = {1, 1, 0},
  [2] = {0, 1, 1},
  [3] = {0, 0, 1},
  [4] = {1, 0.5, 0},
  [5] = {1, 0, 0},
  [6] = {0.2, 1, 0.2},
  [7] = {1, 0, 1},
  [10] = {0.5, 0.5, 0.5}
}

tetris_params = libtetris.tetris_params(libtetris.tetris_inputs( false, false, false, false, false, false, false), 0)

function love.load()
  tetris_game = libtetris.api.create_game()
  libtetris.api.init(tetris_game, 10, 20, 1000000, 166667, 33000)
end

function love.update(dt)
  tetris_params.inputs.rotate_cw = love.keyboard.isDown("up")
  tetris_params.inputs.rotate_ccw = love.keyboard.isDown("lctrl")
  tetris_params.inputs.hold = love.keyboard.isDown("lshift")
  tetris_params.inputs.down = love.keyboard.isDown("down")
  tetris_params.inputs.left = love.keyboard.isDown("left")
  tetris_params.inputs.right = love.keyboard.isDown("right")
  tetris_params.inputs.space = love.keyboard.isDown("space")
  tetris_params.delta_time = dt * 1000000

  rc = libtetris.api.tick(tetris_game, tetris_params)
end

scale = 25
function love.draw()
  love.graphics.setColor(1, 1, 1)
  love.graphics.rectangle("line", scale, scale, 10*scale, 20*scale)
  for y=0,19 do
    for x=0,9 do
      block = libtetris.api.read_game(tetris_game, x, y)
      if(block ~= 0) then
        love.graphics.setColor(colors[block])
        love.graphics.rectangle("fill", scale+x*scale+1, scale+y*scale+1, scale-2, scale-2)
      end
    end
  end
end