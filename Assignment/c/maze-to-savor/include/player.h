#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    int x; // Player's x position in the maze
    int y; // Player's y position in the maze
    int health; // Player's health status
    int score; // Player's score
} Player;

// Function declarations
void movePlayer(Player* player, int deltaX, int deltaY);
void getPlayerStatus(const Player* player);

#endif // PLAYER_H