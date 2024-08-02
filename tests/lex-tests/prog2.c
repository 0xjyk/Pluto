// a random gpt generated c file
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <float.h>

#define BUFFER_SIZE 256
#define MAX_ENTRIES 5

typedef struct {
    char name[BUFFER_SIZE];
    int age;
    float salary;
} Employee;

void initialize_employees(Employee employees[], int size);
void print_employees(const Employee employees[], int size);
void sort_employees_by_age(Employee employees[], int size);
void sort_employees_by_salary(Employee employees[], int size);

int main() {
    Employee employees[MAX_ENTRIES];
    unsigned long large_hex = 0xFFFFFFFFFFFFFFFFUL;
    long long min_long_long = LLONG_MIN;
    float max_float = FLT_MAX;
    double min_double = DBL_MIN;

    initialize_employees(employees, MAX_ENTRIES);
    printf("Original employee list:\n");
    print_employees(employees, MAX_ENTRIES);

    printf("\nSorting employees by age...\n");
    sort_employees_by_age(employees, MAX_ENTRIES);
    print_employees(employees, MAX_ENTRIES);

    printf("\nSorting employees by salary...\n");
    sort_employees_by_salary(employees, MAX_ENTRIES);
    print_employees(employees, MAX_ENTRIES);

    printf("\nConstant values:\n");
    printf("Large hexadecimal value: %#lx\n", large_hex);
    printf("Min long long int: %lld\n", min_long_long);
    printf("Max float: %e\n", max_float);
    printf("Min double: %e\n", min_double);

    return 0;
}

void initialize_employees(Employee employees[], int size) {
    for (int i = 0; i < size; i++) {
        snprintf(employees[i].name, BUFFER_SIZE, "Employee %d", i + 1);
        employees[i].age = 20 + i;
        employees[i].salary = 30000.0f + (float)(i * 5000);
    }
}

void print_employees(const Employee employees[], int size) {
    for (int i = 0; i < size; i++) {
        printf("Name: %s, Age: %d, Salary: %.2f\n", employees[i].name, employees[i].age, employees[i].salary);
    }
}

void sort_employees_by_age(Employee employees[], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (employees[j].age > employees[j + 1].age) {
                Employee temp = employees[j];
                employees[j] = employees[j + 1];
                employees[j + 1] = temp;
            }
        }
    }
}

void sort_employees_by_salary(Employee employees[], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (employees[j].salary > employees[j + 1].salary) {
                Employee temp = employees[j];
                employees[j] = employees[j + 1];
                employees[j + 1] = temp;
            }
        }
    }
}

