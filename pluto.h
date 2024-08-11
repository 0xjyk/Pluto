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

#define MIN_LEVEL CONSTANTS
#define TAB_HASH_SIZE 256

/*
    forward declarations
*/

struct vector;
struct block;
struct location;
struct buffer;
struct token;
struct type;
struct symbol;
union align;
struct node;

typedef struct symbol *Symbol;
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
    // gnu flags - see gnu docs for interpretation
    char nf:1;
    char rtf:1;
    char shf:1;
    char tae:1;
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


typedef struct token {
    int type; 
    int subtype;
    union {
        union {
            unsigned long long int ull; 
            long long int ll; 
            unsigned long int ul; 
            long int l; 
            unsigned int u;
            int i;
        } intval;
        union {
            long double ld; 
            double d; 
            float f;
        } floatval;
        union {
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

typedef struct token_store {
    Token tok; 
    struct token_store *nxt;
} token_store;

enum {PERM=0, FUNC, STMT};
enum {
#define TOKEN(a,b,c) a=b,
#define KEY(a,b,c) a=b,
#include "include/token.h"
#undef TOKEN
#undef KEY
};

typedef union constantval {
    unsigned char uc; 
    signed char c;
    short sh;
    unsigned short ush;
    int i;
    unsigned int ui;
    long int l;
    unsigned long int ul;
    long long int ll;
    unsigned long long int ull;
    float f; 
    double d;
    long double ld;
    void *p;
} constantval;



typedef struct type *Type;
typedef struct type {
    int id;
    Type t;
    int align;
    int size;
    union {
        // sym is used for structs, enum, arrays
        Symbol sym;
        // function type
    } u;

} type;

enum {CONSTANTS=1, LABELS, GLOBAL, PARAM, LOCAL};
typedef struct symbol {
    char *name;
    int scope;
    location loc;
    Symbol prev;
    Type type;
    int sclass;
    // value ?? TODO - update gradually 
    union {
        struct {
            constantval v; 
            Symbol s;
        }c;
    } u;
} symbol;

struct table;
typedef struct table *Table;
typedef struct table {
    unsigned int level;
    // enclosing scope
    Table prev;
    // hash table of symbols in the current scope
    struct entry {
        symbol sym;
        struct entry *next;
    } *buckets[TAB_HASH_SIZE];
    // chain of symbols int level and enclosing scopes
    Symbol all;
} table;

enum {
#define NODE(a,b,c) a=b,
#include "include/node.h"
#undef NODE
};


typedef struct node *Node;
typedef struct node {
    int id;
    int subid;
    int subsubid;
    Type typ;
    int num_kids;
    Node nxt;
    char err:1;
    Node kids;
    union {
        union {
            unsigned long long int ull; 
            long long int ll; 
            unsigned long int ul; 
            long int l; 
            unsigned int u;
            int i;
        } intval;
        union {
            long double ld; 
            double d; 
            float f;
        } floatval;
        union {
            int c; 
            // char 16,32 & wc not supported
        } charval; 
        char *strval;
    } val;

} node;


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
extern Table constants;
extern Table external;
extern Table globals;
extern Table identifiers;
extern Table lables;
// TODO: types
extern Table types;
extern unsigned int level;

extern Node root;
extern token_store *ts;


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
int hash(unsigned char *str, int num_buckets); 
char *make_string(char *str, int len);
char *dtos(int n);
void dump_stringpool();

// read.c 
void fillbuf(); 
void bufinit(char *pp_file);
void ensure_buflen(int len);

// lex.c 
Token lex();
void lexdriver(char *pp_file);

// parse.c 
Node parse(char *pp_file);

// error.c 
void warning(Location l, const char *msg);
void error(Location l, const char *msg);

// sym.c
#endif
