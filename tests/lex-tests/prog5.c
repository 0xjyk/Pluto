// a random gpt generated c file
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR_LEN 100
#define NUM_STRINGS 5

void concatenate_strings(char **strings, int num_strings, char *result);

int main() {
    char *strings[NUM_STRINGS] = {"Hello", "world,", "this", "is", "C"};
    char result[MAX_STR_LEN * NUM_STRINGS] = {0};

    concatenate_strings(strings, NUM_STRINGS, result);

    printf("Concatenated string: %s\n", result);

    return 0;
}

void concatenate_strings(char **strings, int num_strings, char *result) {
    for (int i = 0; i < num_strings; i++) {
        strcat(result, strings[i]);
        if (i < num_strings - 1) {
            strcat(result, " ");
        }
    }
}

