#include <stdio.h>
#include <stdlib.h>
#include "bawana.h"
#include "grid.h"

#define BAWANA_F 0
#define BAWANA_ENTRANCE_W 9
#define BAWANA_ENTRANCE_L 19

int bawana_cells_w[12];
int bawana_cells_l[12];
BawanaType bawana_types[12];
int bawana_mp_gain[12];

void init_bawana() {
    int idx=0;
    for(int w=7; w<=9; w++){
        for(int l=21; l<=24; l++){
            bawana_cells_w[idx]=w;
            bawana_cells_l[idx]=l;
            grid[0][w][l] = CELL_OTHER;
            idx++;
        }
    }
    // Shuffle and assign types
    int pool[12];
    for(int i=0;i<12;i++) pool[i]=i;
    for(int i=0;i<12;i++){
        int t = rand()%5;
        bawana_types[i] = t;
        bawana_mp_gain[i] = 5 + rand()%16;
    }
}

int random_bawana_index() { return rand()%12; }

void apply_bawana_effect(Player *p, int cell_idx, Player players[NUM_PLAYERS]){
    BawanaType type = bawana_types[cell_idx];
    switch(type){
        case BAWANA_FOOD_POISON: p->food_poisoned_turns=3; break;
        case BAWANA_DISORIENTED: p->disoriented_turns=3; break;
        case BAWANA_TRIGGERED: p->triggered_turns=3; break;
        case BAWANA_HAPPY: p->movement_points+=10; break;
        case BAWANA_MP_GAIN: p->movement_points+=bawana_mp_gain[cell_idx]; break;
    }
}

void transport_to_bawana(Player *p, Player players[NUM_PLAYERS]){
    int idx = random_bawana_index();
    p->w = bawana_cells_w[idx];
    p->l = bawana_cells_l[idx];
    p->f = BAWANA_F;
    apply_bawana_effect(p, idx, players);
    printf("Player %c transported to Bawana at [%d,%d,%d]\n", p->name,p->f,p->w,p->l);
}
