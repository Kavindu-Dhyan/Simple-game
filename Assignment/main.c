#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Structs
struct Player
{
    int direction;
    int currentFloor;
    int currentWidth;
    int currentLength;
    int isPlayerActive;
    int throwCount;
};

struct Stair
{
    int startFloor;
    int startWidth;
    int startLength;
    int endFloor;
    int endWidth;
    int endLength;
};

struct Wall
{
    int floor;
    int startWidth;
    int startLength;
    int endWidth;
    int endLength;
};

struct Pole
{
};

// Global variables
struct Stair *stairs = NULL;
int numStairs = 0;

struct Wall *walls = NULL;
int numWalls = 0;

int flag[3];

// Load stairs
void loadStairs()
{
    FILE *file = fopen("stairs.txt", "r");
    if (!file)
    {
        perror("Error opening stairs.txt");
        exit(1);
    }

    int capacity = 10;
    int count = 0;
    stairs = malloc(capacity * sizeof(struct Stair));
    if (!stairs)
    {
        perror("Memory allocation failed");
        exit(1);
    }

    while (fscanf(file, " [%d , %d , %d , %d , %d , %d] ",
                  &stairs[count].startFloor,
                  &stairs[count].startWidth,
                  &stairs[count].startLength,
                  &stairs[count].endFloor,
                  &stairs[count].endWidth,
                  &stairs[count].endLength) == 6)
    {
        count++;
        if (count >= capacity)
        {
            capacity *= 2;
            stairs = realloc(stairs, capacity * sizeof(struct Stair));
            if (!stairs)
            {
                perror("Memory reallocation failed");
                exit(1);
            }
        }
    }

    fclose(file);
    numStairs = count;
}

// Load walls
void loadWalls()
{
    FILE *file = fopen("walls.txt", "r");
    if (!file)
    {
        perror("Error opening walls.txt");
        exit(1);
    }

    int capacity = 10;
    int count = 0;
    walls = malloc(capacity * sizeof(struct Wall));
    if (!walls)
    {
        perror("Memory allocation failed");
        exit(1);
    }

    while (fscanf(file, " [%d , %d , %d , %d , %d] ",
                  &walls[count].floor,
                  &walls[count].startWidth,
                  &walls[count].startLength,
                  &walls[count].endWidth,
                  &walls[count].endLength) == 5)
    {
        // Stop if wall is diagonal
        if (walls[count].startWidth != walls[count].endWidth &&
            walls[count].startLength != walls[count].endLength)
        {
            fprintf(stderr, "Error: Diagonal walls are not allowed! Wall at floor %d from (%d,%d) to (%d,%d)\n",
                    walls[count].floor,
                    walls[count].startWidth, walls[count].startLength,
                    walls[count].endWidth, walls[count].endLength);
            fclose(file);
            exit(1);
        }

        count++;
        if (count >= capacity)
        {
            capacity *= 2;
            walls = realloc(walls, capacity * sizeof(struct Wall));
            if (!walls)
            {
                perror("Memory reallocation failed");
                exit(1);
            }
        }
    }

    fclose(file);
    numWalls = count;
}

// Load flag
void loadFlag()
{
    FILE *file = fopen("flag.txt", "r");
    if (!file)
    {
        perror("Error opening flag.txt");
        exit(1);
    }

    if (fscanf(file, " [%d , %d , %d] ", &flag[0], &flag[1], &flag[2]) != 3)
    {
        fprintf(stderr, "Error: invalid flag format in flag.txt\n");
        fclose(file);
        exit(1);
    }

    fclose(file);
}

// Get seed
int getSeed()
{
    FILE *file;
    unsigned int seed;

    file = fopen("seed.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    fscanf(file, "%u", &seed);
    fclose(file);

    return seed;
}

// Check if a position is blocked by any wall
bool isBlockedByWall(int floor, int width, int length)
{
    for (int i = 0; i < numWalls; i++)
    {
        if (walls[i].floor == floor)
        {
            // Vertical wall
            if (walls[i].startWidth == walls[i].endWidth)
            {
                if (width == walls[i].startWidth &&
                    length >= walls[i].startLength &&
                    length <= walls[i].endLength)
                {
                    return true;
                }
            }
            // Horizontal wall
            else if (walls[i].startLength == walls[i].endLength)
            {
                if (length == walls[i].startLength &&
                    width >= walls[i].startWidth &&
                    width <= walls[i].endWidth)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

// Move player north
void movePlayerNorth(struct Player *player, int diceValue, int northBound)
{
    int newWidth = player->currentWidth - diceValue;

    if (newWidth < northBound)
    {
        printf("Move out of bounds!\n");
        return;
    }

    if (isBlockedByWall(player->currentFloor, newWidth, player->currentLength))
    {
        printf("Move blocked by a wall!\n");
        return;
    }

    player->currentWidth = newWidth;
    printf("Player moved north to width %d\n", player->currentWidth);
}

void movePlayerEast() {}
void movePlayerSouth() {}
void movePlayerWest() {}

int rollDice()
{
    return (rand() % 6) + 1;
}

void setPlayerDirection(struct Player *player)
{
    int dieValue = rollDice();
    if (dieValue != 1 && dieValue != 6)
    {
        player->direction = dieValue;
    }
}

void movePlayers(struct Player players[], int numPlayers)
{

    for (int i = 0; i < numPlayers; i++)
    {
        int dice = rollDice();
        players[i].throwCount++;

        // Change Direction
        if (players[i].throwCount > 0 && players[i].throwCount % 4 == 0)
        {
            int newDirection = rollDice();

            if (newDirection != 1 && newDirection != 6)
            {
                players[i].direction = newDirection;
            }
        }

        printf("Player %d rolled %d\n", i + 1, dice);

        // CHeck if player is active
        if (!players[i].isPlayerActive)
        {
            if (dice == 6)
            {
                players[i].isPlayerActive = true;
                printf("Player %d becomes active!\n", i + 1);
            }
            else
            {
                printf("Player %d is still inactive.\n", i + 1);
                continue;
            }
        }
        // Insert function here to set the players north, east, south, west bounds based on floor

        // Move player based on current direction
        switch (players[i].direction)
        {
        case 2: // North
            movePlayerNorth(&players[i], dice, 0);
            break;
        case 3: // East
            movePlayerEast(&players[i], dice);
            break;
        case 4: // South
            movePlayerSouth(&players[i], dice);
            break;
        case 5: // West
            movePlayerWest(&players[i], dice);
            break;
        default:
            printf("Player %d does not move this turn.\n", i + 1);
            break;
        }
    }
}

int play()
{
    loadWalls();
    loadStairs();
    loadFlag();

    struct Player players[3] = {
        {2, 1, 5, 12, false, 0}, // A
        {5, 1, 9, 7, false, 0},  // B
        {3, 1, 9, 16, false, 0}  // C
    };

    movePlayers(players, 3);

    // Free memory
    free(stairs);
    free(walls);
}
// Main
int main()
{

    // Set the seed
    srand(getSeed());

    play();

    return 0;
}
