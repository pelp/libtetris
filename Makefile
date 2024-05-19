CC=gcc
CC_FLAGS=-c
EXE_NAME=tetris
OUTPUT_DIR=output
SRC_DIR=src
BUILD_DIR=build
EMSDK_REPO=https://github.com/emscripten-core/emsdk.git

SHELL:=/bin/env bash

all: link

output:
	mkdir -p $(OUTPUT_DIR)/www
	mkdir $(BUILD_DIR)



$(BUILD_DIR)/libtetris.o: $(SRC_DIR)/libtetris.c
	cd $(BUILD_DIR) && $(CC) $(CC_FLAGS) ../$(SRC_DIR)/libtetris.c

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c
	cd $(BUILD_DIR) && $(CC) $(CC_FLAGS) ../$(SRC_DIR)/main.c

link: output $(BUILD_DIR)/main.o $(BUILD_DIR)/libtetris.o
	$(CC) $(BUILD_DIR)/main.o $(BUILD_DIR)/libtetris.o -lncurses -o $(OUTPUT_DIR)/$(EXE_NAME)

wasm: output font $(OUTPUT_DIR)/www/index.html $(OUTPUT_DIR)/www/style.css $(OUTPUT_DIR)/www/game.js venv

font:
	cp $(SRC_DIR)/digital-7.mono.ttf $(OUTPUT_DIR)/www/

$(OUTPUT_DIR)/www/style.css: $(SRC_DIR)/html_template/style.css
	cp $(SRC_DIR)/html_template/style.css $(OUTPUT_DIR)/www/style.css
$(OUTPUT_DIR)/www/game.js: $(SRC_DIR)/html_template/game.js
	cp $(SRC_DIR)/html_template/game.js $(OUTPUT_DIR)/www/game.js
$(OUTPUT_DIR)/www/index.html: emsdk $(SRC_DIR)/html_template/template.html
	source $(BUILD_DIR)/emsdk/emsdk_env.sh && emcc $(SRC_DIR)/libtetris.c $(SRC_DIR)/wasm.c -o $(OUTPUT_DIR)/www/index.html --shell-file $(SRC_DIR)/html_template/template.html -s NO_EXIT_RUNTIME=1 -s "EXPORTED_RUNTIME_METHODS=['ccall']"

emsdk:
ifeq ("$(wildcard $(BUILD_DIR)/emsdk)","")
	@echo "Cloning EMSDK repo"
	git clone $(EMSDK_REPO) $(BUILD_DIR)/emsdk
else
	@echo "Updating EMSDK repo"
	cd $(BUILD_DIR)/emsdk && git pull
endif
	$(BUILD_DIR)/emsdk/emsdk install latest
	$(BUILD_DIR)/emsdk/emsdk activate latest

clean:
	rm $(BUILD_DIR)/*.o
	rm -rf $(OUTPUT_DIR)

venv:
ifeq ("$(wildcard $(BUILD_DIR)/.venv)","")
	python3 -m venv $(BUILD_DIR)/.venv
else
	@echo "Virtual environment exists."
endif
	source $(BUILD_DIR)/.venv/bin/activate && pip install websockets
	@echo "To run the websocket server run \`make start\`"

start:
	source $(BUILD_DIR)/.venv/bin/activate && python3 src/db.py
