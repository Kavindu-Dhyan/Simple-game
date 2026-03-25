#include "type.h"
#include "logic.h"

int main() {
    int seed = 12345;
    srand(seed); // Seed the random number generator

    initGame();
    for (int i =0; i < PLAYERS; i++) {
        moveLogic(&player[i]);
    }

    return 0;
}