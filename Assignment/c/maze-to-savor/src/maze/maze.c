#include <stdio.h>
#include <stdlib.h>
#include "maze.h"

#define MAZE_WIDTH 10
#define MAZE_HEIGHT 10

typedef struct {
    int width;
    int height;
    char **grid;
} Maze;

Maze* createMaze(int width, int height) {
    Maze *maze = (Maze *)malloc(sizeof(Maze));
    maze->width = width;
    maze->height = height;
    maze->grid = (char **)malloc(height * sizeof(char *));
    
    for (int i = 0; i < height; i++) {
        maze->grid[i] = (char *)malloc(width * sizeof(char));
        for (int j = 0; j < width; j++) {
            maze->grid[i][j] = (rand() % 2) ? '#' : ' '; // Randomly generate walls and paths
        }
    }
    
    return maze;
}

void freeMaze(Maze *maze) {
    for (int i = 0; i < maze->height; i++) {
        free(maze->grid[i]);
    }
    free(maze->grid);
    free(maze);
}

int isMoveValid(Maze *maze, int x, int y) {
    if (x < 0 || x >= maze->width || y < 0 || y >= maze->height) {
        return 0; // Out of bounds
    }
    return maze->grid[y][x] == ' '; // Check if the move is to a valid path
}

void printMaze(Maze *maze) {
    for (int i = 0; i < maze->height; i++) {
        for (int j = 0; j < maze->width; j++) {
            printf("%c", maze->grid[i][j]);
        }
        printf("\n");
    }
}