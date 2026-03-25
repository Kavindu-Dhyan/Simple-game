CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -pedantic
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
TARGET = $(BIN_DIR)/maze_to_savor

SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/game/game.c \
          $(SRC_DIR)/maze/maze.c \
          $(SRC_DIR)/player/player.c \
          $(SRC_DIR)/utils/utils.c

OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean