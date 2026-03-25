#ifndef BAWANA_H
#define BAWANA_H

#include "players.h"

typedef enum{
    BAWANA_FOOD_POISON = 0,
    BAWANA_DISORIENTED,
    BAWANA_TRIGGERED,
    BAWANA_HAPPY,
    BAWANA_MP_GAIN
} BawanaType;

void init_bawana(void);
int random_bawana_index(void);
void apply_bawana_effect(Player *p, int cell_idx, Player players[NUM_PLAYERS]);
void transport_to_bawana(Player *p, Player players[NUM_PLAYERS]);
void bawana_post_poison_recovery(Player *p, Player players[NUM_PLAYERS]);

#endif
