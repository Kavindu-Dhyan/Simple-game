#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FLOORS 3
#define W 10
#define L 25
#define NUM_PLAYERS 3
#define MAX_STAIRS 20
#define MAX_POLES 20
#define MAX_WALLS 20
// Bawana constants
#define BAWANA_F 0


typedef enum {
    CELL_EMPTY = 0,
    CELL_FLOOR,
    CELL_START_AREA,
    CELL_START_POS,
    CELL_BRIDGE,
    CELL_STANDING,
    CELL_WALL,
    CELL_STAIR,
    CELL_POLE,
    CELL_OTHER,
    CELL_PLAYER_A,
    CELL_PLAYER_B,
    CELL_PLAYER_C,
    CELL_FLAG
} CellType;

CellType grid[FLOORS][W][L];
int cell_effect[FLOORS][W][L];   //stores consumables/bonus/multiplier values

const char *cell_chr[] = {
    " ", ".", "S", "P", "=", "s","W","T","P", "?", "A", "B", "C", "F"
};

typedef enum { NORTH, SOUTH, EAST, WEST } Direction;

// Convert direction enum to string
const char* dir_to_str(Direction d) {
    switch(d) {
        case NORTH: return "North";
        case SOUTH: return "South";
        case EAST:  return "East";
        case WEST:  return "West";
    }
    return "?";
}
  // bawana types
typedef enum{
    BAWANA_FOOD_POISON = 0,
    BAWANA_DISORIENTED,
    BAWANA_TRIGGERED,
    BAWANA_HAPPY,
    BAWANA_MP_GAIN
}BawanaType;

//Bawana arrays
int bawana_cells_w[12];
int bawana_cells_l[12];
BawanaType bawana_types[12];
int bawana_mp_gain[12];

// Rule 13: limit movement points to 250
void cap_movement_points(int *mp) {
    if(*mp > 250) *mp = 250;
}

// player structure 

typedef struct {
    char name;           // 'A', 'B', 'C'
    int w, f, l;         // width, current floor, length
    int first_w, first_l;
    Direction dir;
    int in_maze;          // 0 = outside, 1 = inside
    int throws; 
    int movement_points;   //movement oints in rule 8          // number of dice throws made
    //Bawana related statuses
    int food_poisoned_turns;   //remaining turns that will be missed
    int disoriented_turns;     //remaning randomly moving turns
    int triggered_turns;       //remaing turns with double movements

} Player;


//load seed from seed.txt file
void load_seed(const char *filename) {
    FILE *fp = fopen(filename, "r");
    int seed;
    if (fp == NULL) {
        perror("error opening seed.txt file");
        seed = time(NULL);
    } 
    else {
        fscanf(fp, "%d", &seed);
        printf("seed.txt opened successfully\n");
        fclose(fp);
    }  
    srand(seed);
}

// Initialize grid and cell effects (Rules 8–10)
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
                else cell_effect[f][i][j] = -(2 + rand()%2); // multiplier encoded as negative (-2 or -3)
            }
}


//Stair structure

typedef struct{
    int sf,sw,sl,ef,ew,el;
    int stair_direction;  //0=sf->ef, 1=ef->sf  //start floor, width, length; end floor, width, length
} Stair;

Stair stairs[MAX_STAIRS];
int stair_count = 0;

void load_stairs(const char *filename){
    FILE *fp = fopen(filename,"r");
    if (fp == NULL){
        perror("Error opening file stairs.txt");
        return;
    }
    //read stairs from file
    while(fscanf(fp,"%d %d %d %d %d %d",
                 &stairs[stair_count].sf,&stairs[stair_count].sw,&stairs[stair_count].sl,
                 &stairs[stair_count].ef,&stairs[stair_count].ew,&stairs[stair_count].el)==6){
        if(stair_count < MAX_STAIRS){  
            // validate bounds before placing
            if(stairs[stair_count].sf >= 0 && stairs[stair_count].sf < FLOORS &&
               stairs[stair_count].ef >= 0 && stairs[stair_count].ef < FLOORS &&
               stairs[stair_count].sw >= 0 && stairs[stair_count].sw < W &&
               stairs[stair_count].ew >= 0 && stairs[stair_count].ew < W &&
               stairs[stair_count].sl >= 0 && stairs[stair_count].sl < L &&
               stairs[stair_count].el >= 0 && stairs[stair_count].el < L &&
               (grid[stairs[stair_count].sf][stairs[stair_count].sw][stairs[stair_count].sl] == CELL_STANDING ||
                grid[stairs[stair_count].sf][stairs[stair_count].sw][stairs[stair_count].sl] == CELL_BRIDGE) &&
               (grid[stairs[stair_count].ef][stairs[stair_count].ew][stairs[stair_count].el] == CELL_STANDING ||
                grid[stairs[stair_count].ef][stairs[stair_count].ew][stairs[stair_count].el] == CELL_BRIDGE) &&
               grid[stairs[stair_count].sf][stairs[stair_count].sw][stairs[stair_count].sl] != CELL_WALL &&
               grid[stairs[stair_count].ef][stairs[stair_count].ew][stairs[stair_count].el] != CELL_WALL)
            {
                grid[stairs[stair_count].sf][stairs[stair_count].sw][stairs[stair_count].sl] = CELL_STAIR;
                grid[stairs[stair_count].ef][stairs[stair_count].ew][stairs[stair_count].el] = CELL_STAIR; 
                stairs[stair_count].stair_direction = rand()%2;  //randomly assign stair direction
                stair_count++;
                printf("stairs.txt opened successfully\n");
            } else {
                printf("Ignoring invalid stair entry in stairs.txt\n");   
            }
        }
    }
    fclose(fp); 
}



//flip stairs direction every five turns (rule 6)
void flip_stairs() {
    for(int i=0;i<stair_count;i++){
        stairs[i].stair_direction = rand()%2;
    }
    printf("Stairs direction changed for this round!\n");
}

//Pole structure

typedef struct{
    int sf,ef,w,l;     //start floor, end floor, width, length
} Pole;

Pole poles[MAX_POLES];
int pole_count=0;

void load_poles(const char *filename){
    FILE *fp = fopen(filename,"r");
    if (fp == NULL){
        perror("Error opening file poles.txt");
        return;
    }
    while(fscanf(fp,"%d %d %d %d",
                 &poles[pole_count].sf,
                 &poles[pole_count].ef,
                 &poles[pole_count].w,
                 &poles[pole_count].l) == 4) 
    {
        if(pole_count < MAX_POLES){
            if(poles[pole_count].sf >= 0 && poles[pole_count].sf < FLOORS &&
               poles[pole_count].ef >= 0 && poles[pole_count].ef < FLOORS &&
               poles[pole_count].w >= 0 && poles[pole_count].w < W &&
               poles[pole_count].l >= 0 && poles[pole_count].l < L &&
               (grid[poles[pole_count].sf][poles[pole_count].w][poles[pole_count].l] == CELL_STANDING ||
                grid[poles[pole_count].sf][poles[pole_count].w][poles[pole_count].l] == CELL_BRIDGE) &&
               (grid[poles[pole_count].ef][poles[pole_count].w][poles[pole_count].l] == CELL_STANDING ||
                grid[poles[pole_count].ef][poles[pole_count].w][poles[pole_count].l] == CELL_BRIDGE) &&
               grid[poles[pole_count].sf][poles[pole_count].w][poles[pole_count].l] != CELL_WALL &&
               grid[poles[pole_count].ef][poles[pole_count].w][poles[pole_count].l] != CELL_WALL) 
            {
                grid[poles[pole_count].sf][poles[pole_count].w][poles[pole_count].l] = CELL_POLE;
                grid[poles[pole_count].ef][poles[pole_count].w][poles[pole_count].l] = CELL_POLE;
                pole_count++;
                printf("poles.txt file opened successfully\n");
            } else {
                printf("Ignoring invalid pole entry in poles.txt\n");
            }
        }
    }
    fclose(fp);
}


//Wall structure

typedef struct{
    int f,sw,sl,ew,el; //floor, start width, start length, end width, end length
} Wall;

Wall walls[MAX_WALLS];
int wall_count = 0;

//Load walls from file
void load_walls(const char *filename){
    FILE *fp=fopen(filename,"r");
    if (fp == NULL){
    perror("Error opening file walls.txt");
    return;
    }
    printf("walls.txt file opened successfully\n");
    //read walls from file
    int f,w1,l1,w2,l2;
    while(fscanf(fp,"%d %d %d %d %d",&f,&w1,&l1,&w2,&l2)==5){
        if(f<0||f>=FLOORS) continue;   //invalid diagonal floor
        if(w1!=w2 && l1!=l2){
            printf("Ignoring diagonal wall on floor %d from (%d,%d) to (%d,%d)\n",f,w1,l1,w2,l2);
            continue;
        }
        if(w1==w2){    //vertical wall
            int start=l1<l2?l1:l2,end=l1>l2?l1:l2;
            for(int l=start;l<=end;l++) 
                if(w1>=0 && w1<W && l>=0 && l<L && grid[f][w1][l]==CELL_STANDING) //only place wall on standing cells
                    grid[f][w1][l]=CELL_WALL;
        } else {      //horizontal wall
            int start=w1<w2?w1:w2,end=w1>w2?w1:w2;
            for(int w=start;w<=end;w++) 
                if(w>=0 && w<W && l1>=0 && l1<L && grid[f][w][l1]==CELL_STANDING) //only place wall on standing cells
                    grid[f][w][l1]=CELL_WALL;
        }
        if(wall_count<MAX_WALLS){     //store wall in walls array
            walls[wall_count].f=f;
            walls[wall_count].sw=w1; walls[wall_count].sl=l1;
            walls[wall_count].ew=w2; walls[wall_count].el=l2;
            wall_count++;
        }
    
    }
    fclose(fp);
}


//fill rectangle in the grid with a specific cell type
void fill_rect(int f, int w0, int w1, int l0,int l1, CellType t) {
    if (f < 0 || f >= FLOORS) return;
    if (w0 < 0) w0 = 0;
    if (w1 >= W) w1 = W-1;
    if (l0 < 0) l0 = 0;
    if (l1 >= L) l1 = L-1;
    for (int i = w0; i <= w1; ++i)
        for (int j = l0; j <= l1; ++j)
            grid[f][i][j] = t;
}

//Flag structure
typedef struct{
    int f,w,l;
} Flag;

Flag flag;        // stores the flag position
int flag_set = 0; // 0 = not loaded, 1 = loaded

void load_flag(const char *filename){
    FILE *fp = fopen(filename, "r");
    if(fp == NULL){
        printf("Flag file %s not found!\n", filename);
        return;
    }
    if(fscanf(fp,"%d %d %d", &flag.f, &flag.w, &flag.l)==3){
        // Check if the flag is in a valid standing cell AND not inside Bawana
        int in_bawana = (flag.f == BAWANA_F && flag.w >= 7 && flag.w <= 9 && flag.l >= 21 && flag.l <= 24);
        if(flag.f>=0 && flag.f<FLOORS && flag.w>=0 && flag.w<W && flag.l>=0 && flag.l<L &&
           grid[flag.f][flag.w][flag.l]==CELL_STANDING && !in_bawana){
            grid[flag.f][flag.w][flag.l] = CELL_FLAG;
            flag_set = 1;
        } else {
            printf("Invalid flag position in %s -> program terminated\n", filename);
            fclose(fp);
            exit(1);   //TERMINATE IMMEDIATELY IF FLAG POSITION IS INVALID
        }
    } else {
        printf("Error reading flag from %s\n", filename);
        fclose(fp);
        exit(1);   //TERMINATE IMMEDIATELY IF FLAG POSITION IS INVALID
    }
}

// ---------- check_flag Implementation ----------
int check_flag(Player *p) {
    if(flag_set && p->f == flag.f && p->w == flag.w && p->l == flag.l) {
        printf("%c has reached the flag at [%d,%d,%d]!\n", p->name, flag.f, flag.w, flag.l);
        // Optional: implement scoring or reset logic
        return 1;
    }
    return 0;
}

// Function to check winner
int check_winner(Player players[NUM_PLAYERS], int n) {
    for(int i = 0; i < n; i++) {
        if(players[i].f == flag.f && players[i].w == flag.w && players[i].l == flag.l)
            return i+1; // winner index +1
    }
    return 0; // no winner
}


// Print a floor of the grid
void print_floor(int f) {
    printf("Floor %d\n", f);
    printf("   ");
    for (int l=0; l<L; ++l) printf("%1d", l % 10);
    printf("\n");
    for (int w=0; w<W; ++w) {
        printf("%2d ", w);
        for (int l=0; l<L; ++l) {
            CellType c = grid[f][w][l];
            printf("%s", cell_chr[c]);
        }
        printf("\n");
    }
    printf("\n");
}

//Roll movement dice (1-6)
int roll_movement_dice() { return rand()%6 +1;}

//roll direction dice (6 faces; 2 are empty and 4 represent directions)
int roll_direction_dice() {
    int face = rand()%6 + 1;
    switch(face) {
        case 1: return -1;
        case 2: return NORTH;
        case 3: return EAST;
        case 4: return SOUTH;
        case 5: return WEST;
    }
    return -1;
}

//check if move is possible
int can_move(Player *p, int steps, Direction dir) {
    int w=p->w, l=p->l, f=p->f;
    for(int i=0;i<steps;++i) {
        switch(dir) {
            case NORTH: w--; break;
            case SOUTH: w++; break;
            case EAST:  l++; break; 
            case WEST:  l--; break;

        }
// Check boundaries and cell type        
if(w<0||w>=W||l<0||l>=L) return 0;

//fixed bawana walls(cannot move throught these)
//vertical wall from [6,20] to [9,20]
if(l == 20 && w >= 6 && w <= 9 && f == BAWANA_F) return 0;
//horizontal wall from [6,21] to [6,24]
if(w == 6 && l >= 21 && l <= 24 && f == BAWANA_F) return 0;

//Bawana one-way entrance: cannot enter Bawana through it
if(w >= 7 && w <= 9 && l >= 21 && l <= 24 && f == BAWANA_F) return 0; 

//check regular cell type
if(grid[f][w][l] != CELL_STANDING &&grid[f][w][l] != CELL_START_AREA) return 0;
    }
    return 1;
} 

// --- Rule 4 helper functions ---
int check_stair(Player *p) {
    for (int i=0; i<stair_count; i++) {
        Stair s = stairs[i];
        if (p->f==s.sf && p->w==s.sw && p->l==s.sl) {
            printf("%c lands on [%d,%d,%d] which is a stair cell. %c takes the stairs and now placed at [%d,%d,%d] in floor %d.\n",
                   p->name,p->f,p->w,p->l,p->name,s.ef,s.ew,s.el,s.ef);
            p->f = s.ef; p->w = s.ew; p->l = s.el;
            return 1;
        } else if (p->f==s.ef && p->w==s.ew && p->l==s.el) {
            printf("%c lands on [%d,%d,%d] which is a stair cell. %c takes the stairs and now placed at [%d,%d,%d] in floor %d.\n",
                   p->name,p->f,p->w,p->l,p->name,s.sf,s.sw,s.sl,s.sf);
            p->f = s.sf; p->w = s.sw; p->l = s.sl;
            return 1;
        }
    }
    return 0;
}

//--- Rule 5 helper functions ---
void check_capture(Player players[NUM_PLAYERS], Player *mover) {
    for(int i=0;i<NUM_PLAYERS;i++) {
        if(players[i].name != mover->name &&
        players[i].in_maze &&
        players[i].f==mover->f &&
        players[i].w==mover->w &&
        players[i].l==mover->l) {

            printf("player %c has landed on %c. Player %c is captured by player %c and sent back to start area at [%d,%d]\n",
                   mover->name,players[i].name,players[i].name,mover->name,players[i].first_w, players[i].first_l);

             // Remove captured player from grid and reset position to start area      
            grid[players[i].f][players[i].w][players[i].l] = CELL_STANDING;
            players[i].in_maze = 0;
            players[i].w = players[i].first_w;
            players[i].l = players[i].first_l;
        }
    }
} 


int check_pole(Player *p) {
    for (int i=0; i<pole_count; i++) {
        Pole pol = poles[i];
        if (p->w==pol.w && p->l==pol.l && p->f>=pol.sf && p->f<=pol.ef) {
            printf("%c lands on [%d,%d,%d] which is a pole cell. %c slides down and now placed at [%d,%d,%d] in floor %d.\n",
                   p->name,p->f,p->w,p->l,p->name,pol.sf,p->w,p->l,pol.sf);
            p->f = pol.sf;
            return 1;
        }
    }
    return 0;
}

// Entrance cell for Bawana coordinates
/*const int BAWANA_F = 0;*/
const int BAWANA_ENTRANCE_W = 9;
const int BAWANA_ENTRANCE_L = 19;

// Set up the 12 bawana cells and randomly assign types as per rule
void init_bawana() {
    // Bawana occupies floor 0, w 6..9, l 20..22 (4 x 3 = 12 cells)
    int idx = 0;
    for(int w = 7; w <= 9; ++w) {
        for(int l = 21; l <= 24; ++l) {
            bawana_cells_w[idx] = w;
            bawana_cells_l[idx] = l;
            // mark cell on grid as OTHER (so print shows '?')
            grid[0][w][l] = CELL_OTHER;
            idx++;
        }
    }

// ----- Fixed walls -----
    // Horizontal wall: [6,21] to [6,24]
    for (int l = 21; l <= 24; ++l) {
        grid[0][6][l] = CELL_WALL;
    }
    // Vertical wall: [6,20] to [9,20]
    for (int w = 6; w <= 9; ++w) {
        grid[0][w][20] = CELL_WALL;
    }

    // ----- One-way entrance -----
    // [9,19] allows exit only, cannot enter through movement
    grid[0][BAWANA_ENTRANCE_W][BAWANA_ENTRANCE_L] = CELL_FLOOR;


    // Create a list of 12 indices and assign types with required counts
    int pool[12];
    for(int i=0;i<12;i++) pool[i]=i;
    // Shuffle
    for(int i=11;i>0;i--){
        int j = rand()%(i+1);
        int tmp = pool[i]; pool[i]=pool[j]; pool[j]=tmp;
    }
    // 2 Food Poison
    bawana_types[pool[0]] = BAWANA_FOOD_POISON;
    bawana_types[pool[1]] = BAWANA_FOOD_POISON;
    // 2 Disoriented
    bawana_types[pool[2]] = BAWANA_DISORIENTED;
    bawana_types[pool[3]] = BAWANA_DISORIENTED;
    // 2 Triggered
    bawana_types[pool[4]] = BAWANA_TRIGGERED;
    bawana_types[pool[5]] = BAWANA_TRIGGERED;
    // 2 Happy
    bawana_types[pool[6]] = BAWANA_HAPPY;
    bawana_types[pool[7]] = BAWANA_HAPPY;
    // 4 MP gain (10-100)
    for(int i=8;i<12;i++){
        bawana_types[pool[i]] = BAWANA_MP_GAIN;
        bawana_mp_gain[pool[i]] = 10 + rand()%91; // 10..100
    }
    printf("Bawana initialized (12 cells) on floor 0 at w=6..9, l=20..22. Entrance at [%d,%d]\n", BAWANA_ENTRANCE_W, BAWANA_ENTRANCE_L);
}

// Helper to pick a random bawana cell index
int random_bawana_index() {
    return rand()%12;
}

// Apply the bawana cell effect when a player is placed on that cell (or chosen)
void apply_bawana_effect(Player *p, int cell_idx, Player players[NUM_PLAYERS]) {
    BawanaType t = bawana_types[cell_idx];
    int w = bawana_cells_w[cell_idx];
    int l = bawana_cells_l[cell_idx];

    switch(t) {
        case BAWANA_FOOD_POISON:
            // Player is incapacitated and misses the next 3 throws.
            p->food_poisoned_turns = 3;
            printf("%c eats from Bawana and have a bad case of food poisoning. Will need three rounds to recover.\n", p->name);
            // place player on that bawana cell
            p->f = BAWANA_F; p->w = w; p->l = l;
            // mark on grid
            switch (p->name){
                case 'A': grid[p->f][p->w][p->l]=CELL_PLAYER_A; break;
                case 'B': grid[p->f][p->w][p->l]=CELL_PLAYER_B; break;
                case 'C': grid[p->f][p->w][p->l]=CELL_PLAYER_C; break;
            }
            break;

        case BAWANA_DISORIENTED:
            // get 50 MP and placed in entrance cell. moves randomly for next 4 throws, overrides direction dice.
            p->disoriented_turns = 4;
            p->movement_points = 50;
            cap_movement_points(&p->movement_points); // Rule 13
            p->f = BAWANA_F; p->w = BAWANA_ENTRANCE_W; p->l = BAWANA_ENTRANCE_L;
            p->dir = NORTH; // as required: entrance always placed with North direction
            printf("%c eats from Bawana and is disoriented and is placed at the entrance of Bawana with 50 movement points.\n", p->name);
            switch (p->name){
                case 'A': grid[p->f][p->w][p->l]=CELL_PLAYER_A; break;
                case 'B': grid[p->f][p->w][p->l]=CELL_PLAYER_B; break;
                case 'C': grid[p->f][p->w][p->l]=CELL_PLAYER_C; break;
            }
            break;

        case BAWANA_TRIGGERED:
            // get 50 MP, placed in entrance, moves twice as fast for next 4 throws
            p->triggered_turns = 4;
            p->movement_points = 50;
            cap_movement_points(&p->movement_points); // Rule 13
            p->f = BAWANA_F; p->w = BAWANA_ENTRANCE_W; p->l = BAWANA_ENTRANCE_L;
            p->dir = NORTH;
            printf("%c eats from Bawana and is triggered due to bad quality of food. %c is placed at the entrance of Bawana with 50 movement points.\n", p->name, p->name);
            switch (p->name){
                case 'A': grid[p->f][p->w][p->l]=CELL_PLAYER_A; break;
                case 'B': grid[p->f][p->w][p->l]=CELL_PLAYER_B; break;
                case 'C': grid[p->f][p->w][p->l]=CELL_PLAYER_C; break;
            }
            break;

        case BAWANA_HAPPY:
            // 200 mp, placed entrance
            p->movement_points = 200;
            cap_movement_points(&p->movement_points); // Rule 13
            p->f = BAWANA_F; p->w = BAWANA_ENTRANCE_W; p->l = BAWANA_ENTRANCE_L;
            p->dir = NORTH;
            printf("%c eats from Bawana and is happy. %c is placed at the entrance of Bawana with 200 movement points.\n", p->name, p->name);
            switch (p->name){
                case 'A': grid[p->f][p->w][p->l]=CELL_PLAYER_A; break;
                case 'B': grid[p->f][p->w][p->l]=CELL_PLAYER_B; break;
                case 'C': grid[p->f][p->w][p->l]=CELL_PLAYER_C; break;
            }
            break;

        case BAWANA_MP_GAIN:
            // grant MP and place at that cell
            p->movement_points += bawana_mp_gain[cell_idx];
            cap_movement_points(&p->movement_points); // Rule 13
            p->f = BAWANA_F; p->w = w; p->l = l;
            printf("%c eats from Bawana and earns %d movement points and is placed at the [%d,%d].\n",
                   p->name, bawana_mp_gain[cell_idx], p->w, p->l);
            switch (p->name){
                case 'A': grid[p->f][p->w][p->l]=CELL_PLAYER_A; break;
                case 'B': grid[p->f][p->w][p->l]=CELL_PLAYER_B; break;
                case 'C': grid[p->f][p->w][p->l]=CELL_PLAYER_C; break;
            }
            break;
    }
}


// Transport player to Bawana when movement points depleted (or below zero)
void transport_to_bawana(Player *p, Player players[NUM_PLAYERS]) {
    printf("%c movement points are depleted and requires replenishment. Transporting to Bawana.\n", p->name);
    // choose one of 12 cells randomly and apply its effect
    int idx = random_bawana_index();
    apply_bawana_effect(p, idx, players);
}

// After food poisoning finishes, place player on a random bawana cell and apply effect again
void bawana_post_poison_recovery(Player *p, Player players[NUM_PLAYERS]) {
    int idx = random_bawana_index();
    printf("%c is now fit to proceed from the food poisoning episode and now placed on a cell and the effects take place.\n", p->name);
    apply_bawana_effect(p, idx, players);
}

// Move the player (with boundary check, disorientation, triggers, Bawana, stairs/poles, captures, and flag)
void move_player(Player *p, int steps, Player players[NUM_PLAYERS]) {
    int initial_mp = p->movement_points;
    int moved_cells = 0;
    int total_cost = 0;

    // --- Handle disorientation (random direction) ---
    Direction move_dir = p->dir;
    if (p->disoriented_turns > 0) {
        int d = rand() % 4;
        move_dir = (d == 0) ? NORTH : (d == 1) ? EAST : (d == 2) ? SOUTH : WEST;
    }

    // --- Handle triggered state (double steps) ---
    int effective_steps = steps;
    if (p->triggered_turns > 0) effective_steps = steps * 2;

    // --- Can player move? ---
    if (can_move(p, effective_steps, move_dir)) {
        p->movement_points -= 2;   // Deduct cost for attempt
        cap_movement_points(&p->movement_points); // Rule 13
        total_cost += 2;

        // --- If MP depleted immediately ---
        if (p->movement_points <= 0) {
            int final_mp_before_reset = p->movement_points;
            total_cost += (initial_mp - final_mp_before_reset);
            grid[p->f][p->w][p->l] = CELL_STANDING;
            transport_to_bawana(p, players);
            printf("%c moved %d that cost %d movement points and is left with %d and is moving in the %s.\n",
                   p->name, 0, total_cost, p->movement_points, dir_to_str(p->dir));
            return;
        }

        // --- Normal movement loop ---
        grid[p->f][p->w][p->l] = CELL_STANDING;
        moved_cells = 0;
        for (int i = 0; i < effective_steps; ++i) {
            switch (move_dir) {
                case NORTH: p->w--; break;
                case SOUTH: p->w++; break;
                case EAST:  p->l++; break;
                case WEST:  p->l--; break;
            }
            moved_cells++;

            // Apply cell effect
            int eff = cell_effect[p->f][p->w][p->l];
            if (eff >= 0) {
                p->movement_points -= eff;
            } else {
                int old_mp = p->movement_points;
                p->movement_points *= -eff;  // negative encodes multiplier
            }
            cap_movement_points(&p->movement_points); // Rule 13
            total_cost += (eff >= 0) ? eff : abs(p->movement_points - initial_mp);

            // --- Check if MP depleted mid-move ---
            if (p->movement_points <= 0) {
                printf("%c movement points are depleted and requires replenishment. Transporting to Bawana.\n", p->name);
                transport_to_bawana(p, players);
                printf("%c moved %d that cost %d movement points and is left with %d and is moving in the %s.\n",
                       p->name, moved_cells, total_cost, p->movement_points, dir_to_str(p->dir));
                check_capture(players, p);
                return;
            }

            // Check stairs/poles after each step
            check_stair(p);
            check_pole(p);

            // --- Check flag capture after each step ---
            if (check_flag(p)) {
                return;
            }
        }

        // --- Handle disoriented/triggered messaging ---
        if (p->disoriented_turns > 0) {
            printf("%c rolls %d and is disoriented, moves in %s, moved %d cells, now at [%d,%d].\n",
                   p->name, steps, dir_to_str(move_dir), moved_cells, p->w, p->l);
            p->disoriented_turns--;
            if (p->disoriented_turns == 0) {
                printf("%c has recovered from disorientation.\n", p->name);
            }
        } else if (p->triggered_turns > 0) {
            printf("%c is triggered, rolls %d, moves in %s, moved %d cells, now at [%d,%d].\n",
                   p->name, steps, dir_to_str(move_dir), moved_cells, p->w, p->l);
            p->triggered_turns--;
        } else {
            printf("%c moved %d cells and is now at [%d,%d].\n", p->name, moved_cells, p->w, p->l);
        }

        // Place player back on grid
        switch (p->name) {
            case 'A': grid[p->f][p->w][p->l] = CELL_PLAYER_A; break;
            case 'B': grid[p->f][p->w][p->l] = CELL_PLAYER_B; break;
            case 'C': grid[p->f][p->w][p->l] = CELL_PLAYER_C; break;
        }

        // --- Final capture check ---
        check_capture(players, p);
    }
    else {
        // --- Failed move attempt ---
        p->movement_points -= 2;
        cap_movement_points(&p->movement_points); // Rule 13
        total_cost += 2;
        printf("%c rolls %d but cannot move in the %s. Remains at [%d,%d,%d].\n",
               p->name, steps, dir_to_str(p->dir), p->f, p->w, p->l);

        if (p->movement_points <= 0) {
            printf("%c movement points are depleted and requires replenishment. Transporting to Bawana.\n", p->name);
            transport_to_bawana(p, players);
        }
    }
}

//Display flag location
void display_flag() {
    if(flag_set){
        printf("Flag is at [floor=%d, w=%d, l=%d]\n", flag.f, flag.w, flag.l);
    } else {
        printf("No flag set!\n");
    }
}

//Single player turn 
void player_turn(Player *p, Player players[NUM_PLAYERS]){
    int changed_dir = 0;
    int move = roll_movement_dice();
    p->throws++;

    // If player is food poisoned they miss the turn
    if(p->food_poisoned_turns > 0){
        p->food_poisoned_turns--;
        printf("%c is still food poisoned and misses the turn.\n", p->name);
        if(p->food_poisoned_turns == 0){
            // after expiry: placed on one of the 12 cells randomly and the effects are applied
            bawana_post_poison_recovery(p, players);
        }
        return;
    }

    //Player is at starting area
    if(!p->in_maze){
        if(move==6){
            p->in_maze=1;
            p->w=p->first_w; p->l=p->first_l;
            grid[p->f][p->w][p->l]=(p->name=='A')?CELL_PLAYER_A:(p->name=='B')?CELL_PLAYER_B:CELL_PLAYER_C;
            printf("%c is at the starting area and rolls 6 on the movement dice and is placed on [%d,%d]\n", p->name, p->w, p->l);
        } else {
            printf("%c is at the starting area and rolls %d on the movement dice cannot enter the maze.\n", p->name, move);
        }
        return;
    }

    //rolling the direction dice every forth throw(unless disoriented)

    Direction old_dir = p->dir;
    if(p->throws%4==0){
        int new_dir = roll_direction_dice();
        if(new_dir!=-1){
            p->dir = new_dir;
            changed_dir = 1;
        }
    }

    // Determine which direction to use for moving this turn:
    Direction move_dir = p->dir;
    if(p->disoriented_turns > 0) {
        // choose random direction and this overrides direction dice for this and subsequent disoriented turns
        int d = rand()%4;
        move_dir = (d==0)?NORTH:(d==1)?EAST:(d==2)?SOUTH:WEST;
    }

 // Move if possible
    if(can_move(p, move, p->dir)){
        move_player(p, move, players);

        if(p->disoriented_turns > 0) {
            // disoriented turn handled in move_player, a message will be printed
        } else if(p->triggered_turns >0) {
            // triggered turn handled in move_player, a message will be printed
        } else {
            if(changed_dir){
            printf("%c rolls %d on movement dice and %s on direction dice, changes direction to %s and moves %d cells and is now at [%d,%d]\n",
                p->name, move, dir_to_str(p->dir), dir_to_str(p->dir), move, p->w, p->l);
        } else {
            printf("%c rolls %d on movement dice and moves %s by %d cells and is now at [%d,%d]\n",
                p->name, move, dir_to_str(p->dir), move, p->w, p->l);
            }
        }  
    } else {
        if(changed_dir){
            printf("%c rolls %d on the movement dice and cannot move in the %s. Player remains at [%d,%d,%d]\n",
                p->name, move, dir_to_str(p->dir), dir_to_str(p->dir), p->f, p->w, p->l);
        } else {
            printf("%c rolls %d on movement dice and moves %s by %d cells but cannot move\n",
                p->name, move, dir_to_str(p->dir), move);
        }
        // Deduct the 2 movement points for unsuccesful move and print summary
        p->movement_points -=2;
        if(p->movement_points <= 0){
            printf("%c movement points are depleted and requires replenishment. Transporting to Bawana.\n",p->name);
            //compute cost and print before resetting is done in the above sentence.

            int total_cost = 2; // only the dice cost
            printf("%c moved %d cells that cost %d movement points and is left with %d and is moving in the %s.\n",
                   p->name, 0, total_cost, p->movement_points, dir_to_str(p->dir));
            p->w = p->first_w; p->l = p->first_l;
            p->movement_points = 100;
        } else{
            int total_cost = 2;
            printf("%c moved %d cells that cost %d movement points and is left with %d and is moving in the %s.\n",
            p->name, 0, total_cost, p->movement_points, dir_to_str(p->dir));
        }    
    }

}

//check if player lands on special cells (stairs or poles)
void check_special_cells(Player *p){
    for(int i=0;i<stair_count;i++){
        Stair s=stairs[i];
        if(p->f==s.sf && p->w==s.sw && p->l==s.sl){
            printf("%c lands on [%d,%d,%d] which is a stair cell. %c takes the stairs and now placed at [%d,%d,%d] in floor %d.\n",
                   p->name,p->f,p->w,p->l,p->name,s.ef,s.ew,s.el,s.ef);
            p->f=s.ef; p->w=s.ew; p->l=s.el;
        } else if(p->f==s.ef && p->w==s.ew && p->l==s.el){
            printf("%c lands on [%d,%d,%d] which is a stair cell. %c takes the stairs and now placed at [%d,%d,%d] in floor %d.\n",
                   p->name,p->f,p->w,p->l,p->name,s.sf,s.sw,s.sl,s.sf);
            p->f=s.sf; p->w=s.sw; p->l=s.sl;
        }
    }
    for(int i=0;i<pole_count;i++){
        Pole pol=poles[i];
        if(p->w==pol.w && p->l==pol.l && p->f>=pol.sf && p->f<=pol.ef){
            printf("%c lands on [%d,%d,%d] which is a pole cell. %c slides down and now placed at [%d,%d,%d] in floor %d.\n",
                   p->name,p->f,p->w,p->l,p->name,pol.sf,p->w,p->l,pol.sf);
            p->f=pol.sf;
        }
    }
}

// Play one round for all players
void play_round(Player players[NUM_PLAYERS], int round){
    printf("\n--- Round %d ---\n", round);

    if(round % 5 == 0){    // flip stairs direction every five rounds (Rule 6)
        flip_stairs();
    }

    for(int i = 0; i < NUM_PLAYERS; i++)
        player_turn(&players[i], players);

}



int main(void) {

    init_grid();

    //initializing maze layout before loading stuff from the files
     /*----------Floor 0----------*/
    // All standing areas
    fill_rect(0, 0, 9, 0, 24, CELL_STANDING);
    // Start area
    fill_rect(0, 6, 9, 7, 15, CELL_START_AREA);

    /*----------Floor 1----------*/
    fill_rect(1, 0, 9, 0, 7, CELL_STANDING);
    fill_rect(1, 0, 9, 17, 24, CELL_STANDING);
    fill_rect(1, 3, 6, 8, 16, CELL_BRIDGE);

    /*----------Floor 2----------*/
    fill_rect(2, 0, 9, 16, 24, CELL_STANDING);

     /*----------Initialize Bawana (floor 0, w 6..9, l 20..22) ----------*/
    init_bawana();

    /*----------Players----------*/
    Player A = {'A', 6, 0, 11, 5, 11, NORTH,0,0,100};
    Player B = {'B', 9, 0, 7, 9, 6, WEST,0,0,100};
    Player C = {'C', 9, 0, 15, 9, 16, EAST,0,0,100};

    grid[A.f][A.w][A.l] = CELL_PLAYER_A;
    grid[B.f][B.w][B.l] = CELL_PLAYER_B;
    grid[C.f][C.w][C.l] = CELL_PLAYER_C;

    Player players[NUM_PLAYERS] = {A,B,C};

    // Enforce Rule 13: cap movement points at start
    for(int i = 0; i < NUM_PLAYERS; i++) {
        cap_movement_points(&players[i].movement_points);
    }

    //load stairs, poles, walls, and flag from relevent files
    load_flag("C:/Users/Admin/Desktop/Assingment/flag.txt");
    load_stairs("C:/Users/Admin/Desktop/Assingment/stairs.txt");
    load_poles("C:/Users/Admin/Desktop/Assingment/poles.txt");
    load_walls("C:/Users/Admin/Desktop/Assingment/walls.txt");
    load_seed("C:/Users/Admin/Desktop/Assingment/seed.txt");
    
    /*----------Play rounds until a player wins----------*/
    int round = 1;
    int winner = 0;
    while (!winner) {
        play_round(players, round);
        winner = check_winner(players, NUM_PLAYERS); // returns 0 if no winner, else 1..NUM_PLAYERS
        round++;
    }

    printf("Player %d wins after %d rounds!\n",winner, round-1);

    /*----------Print maze after game----------*/
    for (int f=0; f<FLOORS; ++f)
        print_floor(f);

    printf("Player info:\n");
    printf(" A: start [%d,%d,%d], first move [%d,%d], dir=%s\n",
           A.f,A.w,A.l,A.first_w,A.first_l, dir_to_str(A.dir));
    printf(" B: start [%d,%d,%d], first move [%d,%d], dir=%s\n",
           B.f,B.w,B.l,B.first_w,B.first_l, dir_to_str(B.dir));
    printf(" C: start [%d,%d,%d], first move [%d,%d], dir=%s\n",
           C.f,C.w,C.l,C.first_w,C.first_l, dir_to_str(C.dir));

    return 0;
}
