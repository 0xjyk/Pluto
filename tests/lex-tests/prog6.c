// a random gpt generated c file
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846
#define NUM_POINTS 10

typedef struct {
    double x;
    double y;
} Point;

void generate_circle_points(Point *points, int num_points, double radius);
void print_points(const Point *points, int num_points);

int main() {
    Point points[NUM_POINTS];
    double radius = 10.0;

    generate_circle_points(points, NUM_POINTS, radius);

    printf("Circle points:\n");
    print_points(points, NUM_POINTS);

    return 0;
}

void generate_circle_points(Point *points, int num_points, double radius) {
    for (int i = 0; i < num_points; i++) {
        double angle = 2 * PI * i / num_points;
        points[i].x = radius * cos(angle);
        points[i].y = radius * sin(angle);
    }
}

void print_points(const Point *points, int num_points) {
    for (int i = 0; i < num_points; i++) {
        printf("Point %d: (%f, %f)\n", i, points[i].x, points[i].y);
    }
}

