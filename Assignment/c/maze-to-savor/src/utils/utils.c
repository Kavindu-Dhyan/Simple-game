#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"

// Function to generate a random number between min and max
int randomBetween(int min, int max) {
    return rand() % (max - min + 1) + min;
}

// Function to initialize random number generator
void initRandom() {
    srand(time(NULL));
}

// Function to clear input buffer
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Function to get user input with a prompt
void getUserInput(const char *prompt, char *input, size_t size) {
    printf("%s", prompt);
    if (fgets(input, size, stdin) != NULL) {
        clearInputBuffer();
    }
}