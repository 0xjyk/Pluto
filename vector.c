#include "pluto.h"

/*
 * vector.c implements a simple managed array that grows based on the needs
 */


// create a new vector of pointers of size size
Vector vec_init(size_t size) {
    assert(size >=0); 
    // create a vector struct
    Vector vec = malloc(sizeof(vector));
    assert(vec != NULL);
    vec->size = 0; 
    // allocate space for the managed array 
    vec->vec = malloc(sizeof(void *) * size);
    assert(vec->vec != NULL);
    vec->alloc_size = size;
    return vec;
}

// free the managed array and the vec struct
void vec_free(Vector vec) {
    assert(vec != NULL);
    if (vec->vec)
        free(vec->vec); 
    free(vec);
}

// grow/shirk vec to hold size elements 
void vec_resize(Vector vec, size_t size) {
    assert(vec != NULL); 
    assert(size >= 0); 
    // resize
    if (size == 0 && vec->vec) {
        free(vec->vec);
        vec->vec = NULL; 
    } else {
        vec->vec = realloc(vec, sizeof(void *) * size);
        assert(vec->vec != NULL);
    }
    // update struc fields
    vec->alloc_size = size; 
    vec->size = (vec->size > vec->alloc_size) ? vec->alloc_size : vec->size;
}

// add ptr to the end of vec
void vec_pushback(Vector vec, void *ptr) {
    assert(vec != NULL);
    // ensure there's enough space to add ptr
    if (vec->size >= vec->alloc_size) {
        vec_resize(vec, (vec->alloc_size ? vec->alloc_size * 2 : 1));
    }
    vec->vec[vec->size++] = ptr; 
}

// retrieves the ith element form vec if it exists, returns NULL if unsuccessful
void *vec_get(Vector vec, size_t i) {
    if (i < 0 || vec == NULL || vec->vec == NULL || (i >= vec->size))
        return NULL;
    return vec->vec[i];
}

// updates the ith element from vec if i < size - this can be changed easily based on requirements
void *vec_put(Vector vec, size_t i, void *ptr) {
    if (i < 0 || vec == NULL || vec->vec == NULL || (i >= vec->size))
        return NULL;

    return (vec->vec[i] = ptr);
}

