#ifndef GAME_H
#define GAME_H

typedef struct {
    int score;
    int level;
    // Add more game-related attributes here
} Game;

void startGame(Game *game);
void updateGame(Game *game);
void endGame(Game *game);

#endif // GAME_H