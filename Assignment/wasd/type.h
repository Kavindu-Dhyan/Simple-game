#ifndef TYPE_H
#define TYPE_H

#include <stdbool.h>

typedef struct block {
    int floor,width,length;
    char state; // 'a' for active, 'd' for deactive, 's' for start, 'S' stair, 'P' for pole, 'W' for wall, 'F' for flag
} blockType;

typedef struct player {
    blockType start,out,current,next;
    char name,direction;
    int throws;
    bool isActive,reachedFlag; 
} playerType;

typedef struct {
    int startWidth,startLength,outWidth,outLength;
    char name,direction;
} playerInit;

#endif // TYPE_H