#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "LOGIC1.h"

int main() {
    int s=loadSeed("seed.txt");
    if(s==-1) srand((unsigned)time(NULL)); else srand((unsigned)s);

    initBlocks();
    loadWalls("walls.txt");
    loadStairs("stairs.txt");
    loadPoles("poles.txt");
    loadFlag("flag.txt");

    players[0]=(Player){.f=0,.w=6,.l=12,.name='A',.direction='N'};
    players[1]=(Player){.f=0,.w=9,.l=8,.name='B',.direction='W'};
    players[2]=(Player){.f=0,.w=9,.l=16,.name='C',.direction='E'};

    int outW[3]={5,9,9};
    int outL[3]={12,7,17};

    int round=0; bool gameOver=false;
    while(!gameOver && round<500){
        round++;
        for(int i=0;i<PLAYERS;i++){
            Player *p=&players[i];
            if(p->reachedFlag) continue;

            int roll=rollMovementDice();
            if(!p->isActive){
                if(roll==6){
                    p->isActive=true;
                    p->w=outW[i]; p->l=outL[i];
                    printf(\"%c enters maze at [%d,%d,%d].\\n\",p->name,p->f,p->w,p->l);
                } else {
                    printf(\"%c rolls %d and cannot enter.\\n\",p->name,roll);
                }
                continue;
            }

            p->throws++;
            if(p->throws%4==0){
                char d=rollDirectionDice();
                if(d!='\\0'){p->direction=d; printf(\"%c changes direction to %c.\\n\",p->name,d);}
                else printf(\"%c keeps direction %c.\\n\",p->name,p->direction);
            }

            int pathF[64],pathW[64],pathL[64],len=0,ff,fw,fl;
            bool can=canMovePath(p->f,p->w,p->l,p->direction,roll,&ff,&fw,&fl,pathF,pathW,pathL,&len);
            if(!can){
                printf(\"%c cannot move in %c.\\n\",p->name,p->direction);
                continue;
            }
            executeMove(i,pathF,pathW,pathL,len);
            if(p->reachedFlag){gameOver=true;break;}
        }
    }
    printf(\"Game finished after %d rounds.\\n\",round);
    return 0;
}
