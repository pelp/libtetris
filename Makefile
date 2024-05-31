# Directories
SRC_DIR := src
EXAMPLE_DIR := example
BUILD_DIR := build
OUTPUT_DIR := output

# Compiler and flags
CC := gcc
CFLAGS := -Wall -Werror -Wextra -fPIC
LDFLAGS := -shared

EXE_NAME := tetris
LIB_NAME := libtetris.so

SHELL:=/usr/bin/env bash

# Source files and object files
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))


# Default target
all: lib example

lib: $(OUTPUT_DIR)/lib/$(LIB_NAME)

example: $(OUTPUT_DIR)/$(EXE_NAME)

output:
	mkdir -p $(OUTPUT_DIR)/lib
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/main.o: $(EXAMPLE_DIR)/main.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c -o $@ $<

example: output $(BUILD_DIR)/main.o $(BUILD_DIR)/libtetris.o
	$(CC) $(BUILD_DIR)/main.o $(BUILD_DIR)/libtetris.o -lncurses -o $(OUTPUT_DIR)/$(EXE_NAME)

# Rule to build the example
$(OUTPUT_DIR)/$(EXE_NAME): $(OBJS) $(BUILD_DIR)/main.o
	$(CC) $^ -lncurses -o $(OUTPUT_DIR)/$(EXE_NAME)

# Rule to build the shared library
$(OUTPUT_DIR)/lib/$(LIB_NAME): $(OBJS)
	@mkdir -p $(OUTPUT_DIR)/lib
	$(CC) $(LDFLAGS) -o $@ $^

# Rule to build object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR) $(OUTPUT_DIR)

.PHONY: all clean
