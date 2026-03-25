#ifndef MAZE_H
#define MAZE_H

typedef struct {
    int width;
    int height;
    char **grid;
} Maze;

Maze* createMaze(int width, int height);
void freeMaze(Maze *maze);
int isMoveValid(Maze *maze, int x, int y);
void printMaze(Maze *maze);

#endif // MAZE_H