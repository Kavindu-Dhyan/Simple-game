#include <stdio.h>
#include "game/game.h"
#include "maze/maze.h"
#include "player/player.h"
#include "utils/utils.h"

int main() {
    // Initialize the game
    Game game;
    initializeGame(&game);

    // Create the maze
    Maze maze;
    createMaze(&maze);

    // Main game loop
    while (!isGameOver(&game)) {
        // Update game state
        updateGame(&game, &maze);

        // Handle player input
        Player player;
        getPlayerInput(&player);

        // Move player
        movePlayer(&player, &maze);
        
        // Render the current state
        renderGame(&game, &maze, &player);
    }

    // End the game
    endGame(&game);
    return 0;
}