#ifndef PLAYERS_H
#define PLAYERS_H

#include "grid.h"

typedef enum { NORTH, SOUTH, EAST, WEST } Direction;

typedef struct {
    char name;
    int w,f,l;
    int first_w, first_l;
    Direction dir;
    int in_maze;
    int throws;
    int movement_points;
    int food_poisoned_turns;
    int disoriented_turns;
    int triggered_turns;
} Player;

void cap_movement_points(int *mp);
void place_players_on_grid(Player players[NUM_PLAYERS]);
int can_move(Player *p, int steps, Direction dir);
int check_winner(Player players[NUM_PLAYERS], int n);
void display_player_info(Player players[NUM_PLAYERS]);

#endif
