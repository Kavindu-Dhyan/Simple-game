#ifndef TYPE_H
#define TYPE_H

#include <stdbool.h>

#define FLOORS 3
#define WIDTH 10
#define LENGTH 25
#define TOTAL (FLOORS*WIDTH*LENGTH)
#define PLAYERS 3

#define MAX_STAIRS 256
#define MAX_POLES 256
#define MAX_WALLS 256

typedef enum { DIR_NONE=-1, DIR_N=0, DIR_E=1, DIR_S=2, DIR_W=3 } Dir;

extern const char dirChar[4];

typedef struct {
    int f,w,l;
    char state; // 'a' active, 's' start area, 'W' wall, 'F' flag, 'd' disabled
} Block;

typedef struct {
    int sf,sw,sl;
    int ef,ew,el;
} Stair;

typedef struct {
    int lowF, highF;
    int w,l;
} Pole;

typedef struct {
    int f,w,l;
    char name;
    char direction;
    int throws;       // number of dice throws since entering
    bool isActive;    // entered maze?
    bool reachedFlag; // has captured the flag?
} Player;

#endif
