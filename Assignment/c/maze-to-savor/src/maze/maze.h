#ifndef MAZE_H
#define MAZE_H

typedef struct {
    int width;
    int height;
    char **grid; // 2D array representing the maze
} Maze;

// Function declarations
Maze* createMaze(int width, int height);
void destroyMaze(Maze *maze);
int isMoveValid(Maze *maze, int x, int y);
void printMaze(Maze *maze);

#endif // MAZE_H