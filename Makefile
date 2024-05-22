CC=gcc
CC_FLAGS=-c -Werror -Wall -Wextra -fPIC
EMCC_FLAGS=-Wall -Werror -Wextra
EXE_NAME=tetris
LIB_NAME=libtetris.so
SRC_DIR=src
BUILD_DIR=build
OUTPUT_DIR=output

SHELL:=/usr/bin/env bash

all: link

output:
	mkdir -p $(OUTPUT_DIR)/lib
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/libtetris.o: $(SRC_DIR)/libtetris.c
	cd $(BUILD_DIR) && $(CC) $(CC_FLAGS) ../$(SRC_DIR)/libtetris.c

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c
	cd $(BUILD_DIR) && $(CC) $(CC_FLAGS) ../$(SRC_DIR)/main.c

link: output $(BUILD_DIR)/main.o $(BUILD_DIR)/libtetris.o
	$(CC) $(BUILD_DIR)/main.o $(BUILD_DIR)/libtetris.o -lncurses -o $(OUTPUT_DIR)/$(EXE_NAME)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(OUTPUT_DIR)

lib: output $(BUILD_DIR)/libtetris.o
	$(CC) $(BUILD_DIR)/libtetris.o -shared -o $(OUTPUT_DIR)/lib/$(LIB_NAME)