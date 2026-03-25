#include "grid.h"
#include "players.h"
#include "bawana.h"
#include "stairs_poles_walls.h"
#include "flag.h"
#include "gameplay.h"

int main(void) {

    init_grid();

    // Initialize maze layout before loading files
    init_maze_layout();

    // Initialize Bawana
    init_bawana();

    // Initialize players
    Player A = {'A', 6, 0, 11, 5, 11, NORTH,0,0,100};
    Player B = {'B', 9, 0, 7, 9, 6, WEST,0,0,100};
    Player C = {'C', 9, 0, 15, 9, 16, EAST,0,0,100};

    Player players[NUM_PLAYERS] = {A,B,C};

    place_players_on_grid(players);

    // Enforce Rule 13: cap movement points at start
    for(int i = 0; i < NUM_PLAYERS; i++) {
        cap_movement_points(&players[i].movement_points);
    }

    // Load files
    load_flag("C:/Users/Admin/Desktop/Assingment/flag.txt");
    load_stairs("C:/Users/Admin/Desktop/Assingment/stairs.txt");
    load_poles("C:/Users/Admin/Desktop/Assingment/poles.txt");
    load_walls("C:/Users/Admin/Desktop/Assingment/walls.txt");
    load_seed("C:/Users/Admin/Desktop/Assingment/seed.txt");

    // Play rounds until a player wins
    int round = 1;
    int winner = 0;
    while (!winner) {
        play_round(players, round);
        winner = check_winner(players, NUM_PLAYERS);
        round++;
    }

    printf("Player %d wins after %d rounds!\n",winner, round-1);

    // Print final maze
    print_maze();

    // Display player info
    display_player_info(players);

    return 0;
}
