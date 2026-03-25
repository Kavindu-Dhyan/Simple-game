#ifndef GRID_H
#define GRID_H

#include "players.h"

#define FLOORS 3
#define W 10
#define L 25
#define NUM_PLAYERS 3

typedef enum {
    CELL_EMPTY = 0,
    CELL_FLOOR,
    CELL_START_AREA,
    CELL_START_POS,
    CELL_BRIDGE,
    CELL_STANDING,
    CELL_WALL,
    CELL_STAIR,
    CELL_POLE,
    CELL_OTHER,
    CELL_PLAYER_A,
    CELL_PLAYER_B,
    CELL_PLAYER_C,
    CELL_FLAG
} CellType;

extern CellType grid[FLOORS][W][L];
extern int cell_effect[FLOORS][W][L];

void init_grid(void);
void fill_rect(int f, int w0, int w1, int l0,int l1, CellType t);
void print_floor(int f);
void print_maze(void);

#endif
