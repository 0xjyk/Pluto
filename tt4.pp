# 1 "tests/parse-tests/tt4"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "tests/parse-tests/tt4"
# 25 "tests/parse-tests/tt4"
struct Person person = {.age = 30, .name = "John"};
int arr3[5] = {[2] = 20, [4] = 40, 50};

struct Employee {
    struct Person person;
    int id;
};
struct Employee emp = {.id = 1001, .person.name = "Alice", .person.age = 25};

int matrix[3][4] = {
    {1, 2, 3, 4},
    {5, 6, 7, 8},
    {9, 10, 11, 12}
};


struct Student {
    char name[20];
    int grades[3];
};
struct Student student = {"Bob", {85, 90, 78}};

struct Course {
    char name[30];
    struct Student students[2];
};
struct Course course = {
    "C Programming",
    {
        {"Alice", {90, 85, 88}},
        {"Bob", {78, 80, 75}}
    }
};

union Data {
    int i;
    float f;
    char str[20];
};
union Data data = {.str = "Hello"};


struct Buffer {
    int size;
    char data[];
};
struct Buffer *buf = malloc(sizeof(struct Buffer) + 20);
*buf = (struct Buffer){20, "Hello, world!"};
