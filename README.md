# tetris
A simple tetris game made with WASM and C.

All the game logic is written in C and compiled to WASM using emscripten.

To build the transactional version you can play in your terminal simply run:
```bash
  make
  output/tetris
```

To build the WASM version simply run:
```bash
  make wasm
```
The files you need to host are located under `output/www`.

A version of the WASM game is hosted on https://tetris.toastyfiles.com

To make the leaderboard work you can start the database by running:
```bash
  make start
```

When running over https wss://<domain>/ws needs to point to the server running the leaderboard on port 8763. When running over http the game will try to connect to ws://<domain>:8763
