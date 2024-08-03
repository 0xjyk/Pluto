// a random gpt generated c file
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FILENAME "output.txt"
#define ARRAY_SIZE 100
#define MAX_VALUE 1000

void generate_random_numbers(int *array, int size, int max_value);
void save_to_file(const char *filename, int *array, int size);
void load_from_file(const char *filename, int *array, int size);
void print_array(int *array, int size);

int main() {
    int *array = malloc(ARRAY_SIZE * sizeof(int));
    if (array == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    srand(time(NULL));
    generate_random_numbers(array, ARRAY_SIZE, MAX_VALUE);

    printf("Generated array:\n");
    print_array(array, ARRAY_SIZE);

    save_to_file(FILENAME, array, ARRAY_SIZE);

    int *loaded_array = malloc(ARRAY_SIZE * sizeof(int));
    if (loaded_array == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        free(array);
        return 1;
    }

    load_from_file(FILENAME, loaded_array, ARRAY_SIZE);

    printf("Loaded array:\n");
    print_array(loaded_array, ARRAY_SIZE);

    free(array);
    free(loaded_array);

    return 0;
}

void generate_random_numbers(int *array, int size, int max_value) {
    for (int i = 0; i < size; i++) {
        array[i] = rand() % max_value;
    }
}

void save_to_file(const char *filename, int *array, int size) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file");
        exit(1);
    }

    for (int i = 0; i < size; i++) {
        fprintf(file, "%d\n", array[i]);
    }

    fclose(file);
}

void load_from_file(const char *filename, int *array, int size) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        exit(1);
    }

    for (int i = 0; i < size; i++) {
        if (fscanf(file, "%d", &array[i]) != 1) {
            fprintf(stderr, "Failed to read integer from file\n");
            fclose(file);
            exit(1);
        }
    }

    fclose(file);
}

void print_array(int *array, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

