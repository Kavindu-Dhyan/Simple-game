#include <stdio.h>
#include "players.h"
#include "grid.h"

void cap_movement_points(int *mp){
    if(*mp > 250) *mp = 250;
}

void place_players_on_grid(Player players[NUM_PLAYERS]){
    for(int i=0;i<NUM_PLAYERS;i++){
        switch(players[i].name){
            case 'A': grid[players[i].f][players[i].w][players[i].l] = CELL_PLAYER_A; break;
            case 'B': grid[players[i].f][players[i].w][players[i].l] = CELL_PLAYER_B; break;
            case 'C': grid[players[i].f][players[i].w][players[i].l] = CELL_PLAYER_C; break;
        }
    }
}

int check_winner(Player players[NUM_PLAYERS], int n){
    for(int i=0;i<n;i++){
        if(grid[players[i].f][players[i].w][players[i].l] == CELL_FLAG)
            return i+1;
    }
    return 0;
}

void display_player_info(Player players[NUM_PLAYERS]){
    for(int i=0;i<NUM_PLAYERS;i++){
        printf("Player %c: position [%d,%d,%d], MP=%d\n",
               players[i].name, players[i].f, players[i].w, players[i].l, players[i].movement_points);
    }
}
