//#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>

void printf(char *str, ...) {

}

int main() {
    // Seed the random number generator
    srand(time(0));

    // Generate a random number between 0 and 99
    int number = rand() % 100;

    printf("Generated number: %d\n", number);

    // Check if the number is even or odd
    if (number % 2 == 0) {
        printf("%d is even.\n", number);
    } else {
        printf("%d is odd.\n", number);
    }

    return 0;
}