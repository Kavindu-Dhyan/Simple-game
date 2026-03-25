#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "logic.h"

const char dirChar[4] = {'N','E','S','W'};

Block blocks[TOTAL];
Player players[PLAYERS];
Stair stairs[MAX_STAIRS];
int stairCount = 0;
Pole poles[MAX_POLES];
int poleCount = 0;
int flagF=-1, flagW=-1, flagL=-1;

// --- Helpers ---
void getDelta(char d, int *dw, int *dl) {
    *dw = *dl = 0;
    if(d=='N') *dw=-1;
    else if(d=='S') *dw=1;
    else if(d=='E') *dl=1;
    else if(d=='W') *dl=-1;
}

int idx(int f,int w,int l) { return f*(WIDTH*LENGTH) + w*LENGTH + l; }
bool inBounds(int f,int w,int l) { return (f>=0&&f<FLOORS&&w>=0&&w<WIDTH&&l>=0&&l<LENGTH); }

void printCell(int f,int w,int l, char *buf) { sprintf(buf,"[%d,%d,%d]", f,w,l); }

// --- Loaders ---
int loadSeed(const char *filename) {
    FILE *fp=fopen(filename,"r"); if(!fp) return -1;
    int s; if(fscanf(fp,"%d",&s)==1){fclose(fp);return s;}
    fclose(fp); return -1;
}

void loadStairs(const char *filename) {
    FILE *fp=fopen(filename,"r"); if(!fp) return;
    int a,b,c,d,e,f;
    while(fscanf(fp,"%d %d %d %d %d %d",&a,&b,&c,&d,&e,&f)==6){
        stairs[stairCount++] = (Stair){a,b,c,d,e,f};
    }
    fclose(fp);
}

void loadPoles(const char *filename) {
    FILE *fp=fopen(filename,"r"); if(!fp) return;
    int a,b,c,d;
    while(fscanf(fp,"%d %d %d %d",&a,&b,&c,&d)==4){
        poles[poleCount++] = (Pole){a<b?a:b, a<b?b:a, c,d};
    }
    fclose(fp);
}

void loadWalls(const char *filename) {
    FILE *fp=fopen(filename,"r"); if(!fp) return;
    int f,sw,sl,ew,el;
    while(fscanf(fp,"%d %d %d %d %d",&f,&sw,&sl,&ew,&el)==5){
        if(sw==ew){
            for(int l=sl;l<=el;l++) if(inBounds(f,sw,l)) blocks[idx(f,sw,l)].state='W';
        } else if(sl==el){
            for(int w=sw;w<=ew;w++) if(inBounds(f,w,sl)) blocks[idx(f,w,sl)].state='W';
        }
    }
    fclose(fp);
}

void loadFlag(const char *filename) {
    FILE *fp=fopen(filename,"r"); if(!fp) return;
    int f,w,l;
    if(fscanf(fp,"%d %d %d",&f,&w,&l)==3){
        flagF=f;flagW=w;flagL=l;
        blocks[idx(f,w,l)].state='F';
    }
    fclose(fp);
}

// --- Init blocks ---
void initBlocks() {
    for(int f=0;f<FLOORS;f++) for(int w=0;w<WIDTH;w++) for(int l=0;l<LENGTH;l++){
        blocks[idx(f,w,l)] = (Block){f,w,l,'a'};
    }
    // start area (floor 0, width>=6, length 8..16)
    for(int w=6;w<10;w++) for(int l=8;l<=16;l++) blocks[idx(0,w,l)].state='s';
    // disable some floor parts to match description
    for(int w=0;w<6;w++) for(int l=8;l<=16;l++) blocks[idx(1,w,l)].state='d';
    for(int l=0;l<25;l++) if(l<8||l>16) for(int w=0;w<10;w++) blocks[idx(2,w,l)].state='d';
}

// --- Finders ---
int findStairAt(int f,int w,int l){
    for(int i=0;i<stairCount;i++){
        if((stairs[i].sf==f&&stairs[i].sw==w&&stairs[i].sl==l)||
           (stairs[i].ef==f&&stairs[i].ew==w&&stairs[i].el==l)) return i;
    }
    return -1;
}
int findPoleAt(int f,int w,int l){
    for(int i=0;i<poleCount;i++){
        if(poles[i].w==w&&poles[i].l==l&&f>=poles[i].lowF&&f<=poles[i].highF) return i;
    }
    return -1;
}

// --- Movement ---
bool canMovePath(int startF,int startW,int startL,char direction,int steps,
                 int *finalF,int *finalW,int *finalL,
                 int pathF[],int pathW[],int pathL[],int *pathLen) {
    int dw,dl; getDelta(direction,&dw,&dl);
    int cf=startF,cw=startW,cl=startL;
    int plen=0;
    for(int s=0;s<steps;s++){
        int nw=cw+dw,nl=cl+dl,nf=cf;
        if(!inBounds(nf,nw,nl)) return false;
        if(blocks[idx(nf,nw,nl)].state=='W' || blocks[idx(nf,nw,nl)].state=='d') return false;
        cw=nw; cl=nl; cf=nf;
        pathF[plen]=cf; pathW[plen]=cw; pathL[plen]=cl; plen++;
    }
    *finalF=cf;*finalW=cw;*finalL=cl;*pathLen=plen;
    return true;
}

void executeMove(int pIndex,int pathF[],int pathW[],int pathL[],int pathLen) {
    Player *p=&players[pIndex];
    int f=pathF[pathLen-1], w=pathW[pathLen-1], l=pathL[pathLen-1];
    p->f=f;p->w=w;p->l=l;
    char buf[32]; printCell(f,w,l,buf);
    printf(\"%c is now at %s.\\n\",p->name,buf);

    int stairIdx=findStairAt(f,w,l);
    if(stairIdx!=-1){
        Stair st=stairs[stairIdx];
        if(st.sf==f&&st.sw==w&&st.sl==l){p->f=st.ef;p->w=st.ew;p->l=st.el;}
        else {p->f=st.sf;p->w=st.sw;p->l=st.sl;}
        printf(\"%c takes stairs to [%d,%d,%d].\\n\",p->name,p->f,p->w,p->l);
    }
    int poleIdx=findPoleAt(p->f,p->w,p->l);
    if(poleIdx!=-1){
        p->f=poles[poleIdx].lowF;
        printf(\"%c slides down pole to [%d,%d,%d].\\n\",p->name,p->f,p->w,p->l);
    }
    if(p->f==flagF&&p->w==flagW&&p->l==flagL){
        p->reachedFlag=true;
        printf(\"%c captured the flag! Game over.\\n\",p->name);
    }
}

// --- Dice ---
int rollMovementDice(){ return (rand()%6)+1; }
char rollDirectionDice(){
    int face=(rand()%6)+1;
    if(face==2) return 'N';
    if(face==3) return 'E';
    if(face==4) return 'S';
    if(face==5) return 'W';
    return '\\0';
}
