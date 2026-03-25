#include <stdio.h>
#include <stdlib.h>
#include "grid.h"

const char *cell_chr[] = {
    " ", ".", "S", "P", "=", "s","W","T","P", "?", "A", "B", "C", "F"
};

void init_grid(void){
    for(int f=0;f<FLOORS;f++)
        for(int i=0;i<W;i++)
            for(int j=0;j<L;j++){
                grid[f][i][j] = CELL_FLOOR;
                int r = rand()%100;
                if(r<25) cell_effect[f][i][j] = 0;
                else if(r<60) cell_effect[f][i][j] = 1 + rand()%4;
                else if(r<85) cell_effect[f][i][j] = 1 + rand()%2;
                else if(r<95) cell_effect[f][i][j] = 3 + rand()%3;
                else cell_effect[f][i][j] = -(2 + rand()%2);
            }
}

void fill_rect(int f, int w0, int w1, int l0,int l1, CellType t){
    if(f < 0 || f >= FLOORS) return;
    if(w0 < 0) w0 = 0;
    if(w1 >= W) w1 = W-1;
    if(l0 < 0) l0 = 0;
    if(l1 >= L) l1 = L-1;
    for(int i=w0;i<=w1;i++)
        for(int j=l0;j<=l1;j++)
            grid[f][i][j] = t;
}

void print_floor(int f){
    printf("Floor %d\n", f);
    printf("   ");
    for(int l=0;l<L;l++) printf("%1d", l%10);
    printf("\n");
    for(int w=0;w<W;w++){
        printf("%2d ", w);
        for(int l=0;l<L;l++){
            printf("%s", cell_chr[grid[f][w][l]]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_maze(void){
    for(int f=0;f<FLOORS;f++)
        print_floor(f);
}
