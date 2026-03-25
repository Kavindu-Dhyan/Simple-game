#include <stdio.h>
#include "player.h"

typedef struct {
    int x;
    int y;
    int health;
    int score;
} Player;

Player createPlayer(int startX, int startY) {
    Player newPlayer;
    newPlayer.x = startX;
    newPlayer.y = startY;
    newPlayer.health = 100; // Default health
    newPlayer.score = 0;    // Default score
    return newPlayer;
}

void movePlayer(Player *player, int deltaX, int deltaY) {
    player->x += deltaX;
    player->y += deltaY;
}

void getPlayerStatus(Player *player) {
    printf("Player Position: (%d, %d)\n", player->x, player->y);
    printf("Player Health: %d\n", player->health);
    printf("Player Score: %d\n", player->score);
}