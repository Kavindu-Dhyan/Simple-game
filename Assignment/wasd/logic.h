#ifndef LOGIC_H
#define LOGIC_H

#include "type.h"
#include <stdlib.h>

#define FlOOR 3
#define WIDTH 10
#define LENGTH 25
#define PLAYERS 3

#define TOTAL WIDTH*LENGTH*FlOOR

blockType block[TOTAL];
playerType player[PLAYERS];

playerInit playerInitData[PLAYERS] = {
    {6, 12, 5, 12, 'A', 'N'},
    {9, 8, 9, 7, 'B', 'W'},
    {9, 16, 9, 17, 'C', 'E'}
};

void initBlocks() {
    int index = 0;
    for(int f = 0; f < FlOOR; f++) {
        for(int w = 0; w < WIDTH; w++) {
            for(int l = 0; l < LENGTH; l++) {
                block[index].floor = f;
                block[index].width = w;
                block[index].length = l;
                if(
                    block[index].floor == 0 &&
                    block[index].width >= 6 &&
                    block[index].length >= 8 &&
                    block[index].length <= 16
                ) {block[index].state = 's';} // start area
                else if(
                    block[index].floor == 1 &&
                    block[index].width < 6 &&
                    block[index].length >= 8 &&
                    block[index].length <= 16
                ) {block[index].state = 'd';} // above bridge
                else if(
                    block[index].floor == 2 &&
                    (
                    block[index].length < 8 ||
                    block[index].length > 16
                ))  {block[index].state = 'd';} // third floor
                else {block[index].state = 'a';} // active
                index++;
            }
        }
    }
}

void initPlayers() {
    for(int i=0; i < PLAYERS; i++) {
        int startIndex = playerInitData[i].startWidth * LENGTH + playerInitData[i].startLength * WIDTH;
        int outIndex = playerInitData[i].outWidth * LENGTH + playerInitData[i].outLength * WIDTH;
        
        player[i].start = block[startIndex];
        player[i].out = block[outIndex];

        player[i].current = player[i].start;
        player[i].next = player[i].out;
        player[i].name = playerInitData[i].name;
        player[i].direction = playerInitData[i].direction;

        player[i].throws = 0;
        player[i].reachedFlag = false;
    }
}

int diceRoll() {
    return (rand() % 6) + 1; // returns a number between 1 and 6
}

void initGame() {
    initBlocks();
    initPlayers();
}

char directionRoll() {
    int face = diceRoll();
    switch(face) {
        case 2: return 'N';
        case 3: return 'E';
        case 4: return 'S';
        case 5: return 'W';
        default: return '\0'; // default case, should not occur
    }
}

void getDelta(char direction, int *deltaWidth, int *deltaLength) {
    switch(direction) {
        case 'N':
            *deltaWidth = -1;
            *deltaLength = 0;
            break;
        case 'E':
            *deltaWidth = 0;
            *deltaLength = 1;
            break;
        case 'S':
            *deltaWidth = 1;
            *deltaLength = 0;
            break;
        case 'W':
            *deltaWidth = 0;
            *deltaLength = -1;
            break;
        default:
            *deltaWidth = 0;
            *deltaLength = 0; // No movement for invalid direction
            break;
    }
}

void moveLogic(playerType *player) {

    if(!player->isActive) {
        int firstRoll = diceRoll();
        if(firstRoll == 6) {
            player->isActive = true;
            player->current = player->out;
        } else {
            return; // Player remains inactive
        }
    }

    // count if inside maze
    int moveSteps = diceRoll();
    player->throws++;

    bool directionChanged = false;
    if(player->throws % 4 == 0) {
        char newDirection = directionRoll();
        if(newDirection != '\0' && newDirection != player->direction) {
            player->direction = newDirection;
            directionChanged = true;
        }
    }
    
    int deltaWidth = 0, deltaLength = 0;
    getDelta(player->direction, &deltaWidth, &deltaLength);

    int newWidth = player->current.width;
    int newLength = player->current.length;
    int newFloor = player->current.floor;

    for(int step = 0; step < moveSteps; step++) {
        newWidth = newWidth + deltaWidth;
        newLength = newLength + deltaLength;


        // Check for boundaries (24,9) >>> nextBlock;
        // Check disabled blocks state = 'd', 's', 'B'
        // Check walls
        // Check stairs
        // Check poles
        // Check flag
    }

}

#endif // LOGIC_H