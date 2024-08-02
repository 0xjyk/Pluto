// a random gpt generated c file
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define NAME_LENGTH 50

typedef struct {
    char name[NAME_LENGTH];
    int age;
    bool is_student;
} Person;

void initialize_person(Person *person, const char *name, int age, bool is_student);
void print_person(const Person *person);
void swap(int *a, int *b);
void reverse_array(int arr[], int size);

int main() {
    Person person1, person2;
    unsigned int decimal_value = 4294967295u;
    int octal_value = 0777;
    int hex_value = 0xABC;
    long long large_decimal = 9223372036854775807LL;
    float decimal_with_exponent = 1.23e10;

    initialize_person(&person1, "John Doe", 25, true);
    initialize_person(&person2, "Jane Smith", 30, false);

    printf("Person 1:\n");
    print_person(&person1);

    printf("Person 2:\n");
    print_person(&person2);

    int values[] = {1, 2, 3, 4, 5};
    int size = sizeof(values) / sizeof(values[0]);

    printf("\nOriginal array: ");
    for (int i = 0; i < size; i++) {
        printf("%d ", values[i]);
    }
    printf("\n");

    reverse_array(values, size);
    printf("Reversed array: ");
    for (int i = 0; i < size; i++) {
        printf("%d ", values[i]);
    }
    printf("\n");

    printf("\nConstant values:\n");
    printf("Decimal value: %u\n", decimal_value);
    printf("Octal value: %o\n", octal_value);
    printf("Hexadecimal value: %#x\n", hex_value);
    printf("Large decimal value: %lld\n", large_decimal);
    printf("Decimal with exponent: %e\n", decimal_with_exponent);

    return 0;
}

void initialize_person(Person *person, const char *name, int age, bool is_student) {
    strncpy(person->name, name, NAME_LENGTH);
    person->name[NAME_LENGTH - 1] = '\0';
    person->age = age;
    person->is_student = is_student;
}

void print_person(const Person *person) {
    printf("Name: %s, Age: %d, Is Student: %s\n", person->name, person->age, person->is_student ? "Yes" : "No");
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void reverse_array(int arr[], int size) {
    for (int i = 0; i < size / 2; i++) {
        swap(&arr[i], &arr[size - i - 1]);
    }
}

