#ifndef GAME_H
#define GAME_H

typedef struct {
    // Game state variables
    int score;
    int level;
    int isRunning;
} Game;

void startGame(Game *game);
void updateGame(Game *game);
void endGame(Game *game);

#endif // GAME_H