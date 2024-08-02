// a random gpt generated c file
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>

#define ARRAY_SIZE 10

void print_array(int arr[], int size);
void sort_array(int arr[], int size);
int search_array(int arr[], int size, int target);

int main() {
    int array[ARRAY_SIZE] = {1024, 512, 256, 128, 64, 32, 16, 8, 4, 2};
    int target = 128;
    unsigned int max_unsigned = UINT_MAX;
    long min_long = LONG_MIN;
    long long max_long_long = LLONG_MAX;
    int32_t hex_value = 0xDEADBEEF;

    printf("Original array:\n");
    print_array(array, ARRAY_SIZE);

    printf("\nSorting array...\n");
    sort_array(array, ARRAY_SIZE);
    print_array(array, ARRAY_SIZE);

    printf("\nSearching for %d in array...\n", target);
    int index = search_array(array, ARRAY_SIZE, target);
    if (index != -1) {
        printf("Found %d at index %d\n", target, index);
    } else {
        printf("%d not found in array\n", target);
    }

    printf("\nConstant values:\n");
    printf("Max unsigned int: %u\n", max_unsigned);
    printf("Min long int: %ld\n", min_long);
    printf("Max long long int: %lld\n", max_long_long);
    printf("Hexadecimal value: %#x\n", hex_value);

    return 0;
}

void print_array(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void sort_array(int arr[], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int search_array(int arr[], int size, int target) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == target) {
            return i;
        }
    }
    return -1;
}

