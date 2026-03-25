#include <stdio.h>
#include "gameplay.h"
#include "players.h"
#include "bawana.h"

void play_round(Player players[NUM_PLAYERS], int round){
    printf("Round %d\n", round);
    for(int i=0;i<NUM_PLAYERS;i++){
        if(players[i].movement_points <= 0){
            transport_to_bawana(&players[i], players);
        } else {
            printf("Player %c moves...\n", players[i].name);
            // stub for movement logic
            players[i].movement_points--;
        }
    }
}
