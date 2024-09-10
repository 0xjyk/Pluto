typedef unsigned long int size_t;
typedef struct vector {
    size_t size; 
    size_t alloc_size;
    void **vec;
} vector;
typedef vector *Vector;

