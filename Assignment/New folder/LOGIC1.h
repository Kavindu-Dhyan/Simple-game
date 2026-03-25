#ifndef LOGIC_H
#define LOGIC_H

#include "type.h"

extern Block blocks[TOTAL];
extern Player players[PLAYERS];
extern Stair stairs[MAX_STAIRS];
extern int stairCount;
extern Pole poles[MAX_POLES];
extern int poleCount;
extern int flagF, flagW, flagL;

// Initialization
void initBlocks();
void loadWalls(const char *filename);
void loadStairs(const char *filename);
void loadPoles(const char *filename);
void loadFlag(const char *filename);
int loadSeed(const char *filename);

// Helpers
void getDelta(char d, int *dw, int *dl);
int idx(int f, int w, int l);
bool inBounds(int f,int w,int l);
void printCell(int f,int w,int l, char *buf);

int findStairAt(int f,int w,int l);
int findPoleAt(int f,int w,int l);

// Movement
bool canMovePath(int startF,int startW,int startL, char direction, int steps,
                 int *finalF,int *finalW,int *finalL,
                 int pathF[], int pathW[], int pathL[], int *pathLen);
void executeMove(int pIndex, int pathF[], int pathW[], int pathL[], int pathLen);

// Dice
int rollMovementDice();
char rollDirectionDice();

#endif
