#include <stdio.h>
#include "flag.h"
#include "grid.h"

void load_flag(const char *path){
    printf("Loading flag from %s\n", path);
    grid[0][5][24] = CELL_FLAG;
}
