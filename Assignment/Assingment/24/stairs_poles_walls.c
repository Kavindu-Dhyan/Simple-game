#include <stdio.h>
#include <stdlib.h>
#include "stairs_poles_walls.h"
#include "grid.h"

void load_stairs(const char *path) {
    // stub for file reading stairs
    printf("Loading stairs from %s\n", path);
}

void load_poles(const char *path){
    printf("Loading poles from %s\n", path);
}

void load_walls(const char *path){
    printf("Loading walls from %s\n", path);
}

void init_maze_layout(void){
    // Example: fill start positions
    fill_rect(0, 0, 1, 0, 1, CELL_START_AREA);
}
