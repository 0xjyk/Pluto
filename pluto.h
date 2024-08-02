#ifndef PLUTO_H
#define PLUTO_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stddef.h>
#include <uchar.h>
#include <libgen.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>

// pluto.h

// cli flags
#define LEX 0x1
#define PARSE 0x3
#define TYPECHECK 0x5
#define CGEN 0x7
#define SETSCOPE(scope) (CGEN & ~(scope))

#define BUFLEN 4096
#define MAXLINE 4096
#define MINBUFLEN 32
#define MAXSTRLEN 512
#define STRBUFLEN (MAXSTRLEN + 3)
#define BUFSIZE MAXLINE + BUFLEN + 1



/*
 *  type declarations
 */
typedef struct vector {
    size_t size; 
    size_t alloc_size;
    void **vec;
} vector;
typedef vector *Vector;

typedef struct block block;
typedef block *Block; 

struct block {
    Block next; 
    char *avail;
    char *limit;
};

union align {
    long l; 
    char *p; 
    double d; 
    int (*f) (void);
};

typedef struct location {
    char *file;
    int x,y; 
} location; 
typedef location *Location;

typedef struct buffer {
    char *file;
    int fd;
    char buf[BUFSIZE];
    char *curr; 
    char *bufend;
} buffer;
typedef buffer *Buffer;

/*
typedef struct token {
    int type; 
    char *val; 
    union intval {
        unsigned long long int ull; 
        long long int ll; 
        unsigned long int ul; 
        long int l; 
        unsigned int u;
        int i;
    } intval;
    union floatval {
        long double ld; 
        double d; 
        float f; 
    } floatval;
    union charval { 
        int c; 
        char16_t c16; 
        char32_t c32; 
        wchar_t wc;
    } charval;
    char *strval;
    location loc;
} token;
*/

typedef struct token {
    int type; 
    int subtype;
    union val {
        union intval {
            unsigned long long int ull; 
            long long int ll; 
            unsigned long int ul; 
            long int l; 
            unsigned int u;
            int i;
        } intval;
        union floatval {
            long double ld; 
            double d; 
            float f;
        } floatval;
        union charval {
            int c; 
            // char 16,32 & wc not supported
        } charval; 
        char *strval;
    } val;
    // length of the original string before compressing into val
    int len;
    location loc;
} token;

typedef token *Token;


enum {PERM=0, FUNC, STMT};
enum {
#define TOKEN(a,b,c) a=b,
#define KEY(a,b,c) a=b,
#include "include/token.h"
#undef TOKEN
#undef KEY
};

/*
 *  global variables
 */
extern unsigned char JOBSCOPE;
extern int num_files;
extern char **infiles;
static char *outfile = NULL;
static char *cwd = NULL;
static char temp_dir_name[] = "/__temp__/";
static char temp_dir[PATH_MAX];
static char pp_suffix[] = ".pp";
extern Vector tempfiles;
extern Vector ppfiles;
extern location loc;
extern buffer buf;
extern token t;
extern int pp_read_complete;
extern int num_warnings;
extern int num_errors;

/*
 *  forward declartions
 */

// util.c
void parseoptions(int argc, char **argv);
void usage();
void preprocess();
void process();
void remove_tempfiles();

// vector.c
Vector vec_init(size_t size);
void vec_free(Vector vec);
void vec_resize(Vector vec, size_t size);
void vec_pushback(Vector vec, void *ptr); 
void *vec_get(Vector vec, size_t i); 
void *vec_put(Vector vec, size_t i, void *ptr);

// alloc.c
void *alloc(unsigned long sz, int an);
void dealloc(int an);
char *strloc(char *str, int sz, int an);

// string.c 
int hash(unsigned char *str); 
char *make_string(char *str, int len);
void dump_stringpool();

// read.c 
void fillbuf(); 
void bufinit(char *pp_file);
void ensure_buflen(int len);

// lex.c 
Token lex();
void lexdriver(char *pp_file);
/* private functions - not exported; to be deleted
Token is_keyword(); 
Token is_identifier();
Token is_punct();
Token is_constant();
Token is_intconst();
Token is_floatconst(); 
Token is_enumconst() ;
Token is_charconst();
Token is_string();
int is_escapeseq(int *val, char *s);
*/

// parse.c 
void parse(char *pp_file);
void print_cpp();

// error.c 
void warning(Location l, const char *msg);
void error(Location l, const char *msg);

#endif
