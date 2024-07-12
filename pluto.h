#ifndef PLUTO_H
#define PLUTO_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stddef.h>
#include <libgen.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/wait.h>

// pluto.h

// cli flags
#define LEX 0x1
#define PARSE 0x3
#define TYPECHECK 0x5
#define CGEN 0x7
#define SETSCOPE(scope) (CGEN & ~(scope))



/*
 *  type declarations
 */
typedef struct vector {
    size_t size; 
    size_t alloc_size;
    void **vec;
} vector;
typedef vector *Vector;

/*
 *  global variables
 */
static unsigned char JOBSCOPE = 0x7;
static int num_files;
static char **infiles;
static char **ppfiles;
static char *outfile = NULL;
static char *cwd = NULL;
static char temp_dir_name[] = "/__temp__/";
static char temp_dir[PATH_MAX];
static char pp_suffix[] = ".pp";
static Vector tempfiles = NULL;



/*
 *  forward declartions
 */

// util.c
void parseoptions(int argc, char **argv);
void usage();
void preprocess();
void remove_tempfiles();

// vector.c
Vector vec_init(size_t size);
void vec_free(Vector vec);
void vec_resize(Vector vec, size_t size);
void vec_pushback(Vector vec, void *ptr); 
void *vec_get(Vector vec, size_t i); 
void *vec_put(Vector vec, size_t i, void *ptr);




#endif
