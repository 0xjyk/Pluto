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
#include <float.h>
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
struct field;

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
    _Bool b;
    unsigned char uc; 
    signed char sc;
    char c;
    short s;
    unsigned short us;
    signed int i;
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
    Type type;
    int align;
    int size;
    char *strrep;
    unsigned int usignedf:1;
    // used only when array size is non-reducable assign expr
    struct node *arr_size;
    union {
        // sym is used for structs, union (enum?, arrays?)
        Symbol sym;
        // function type
        struct {
            // a null terminated array of types
            Vector proto;
        } f;
    } u;
} type;

typedef struct field *Field;
typedef struct field {
    char *name; 
    Type type;
    location loc;
    char sclass:6;
    int offset; 
    short bitsize;
    Field next;
} field;

enum {CONSTANTS=1, LABELS, GLOBAL, PARAM, LOCAL};
typedef struct symbol {
    char *name;
    int scope;
    location loc;
    Symbol prev;
    Type type;
    char sclass:6;
    unsigned int temporary:1;
    unsigned int generated:1;
    unsigned int defined:1;
    unsigned int addressed:1;
    // value ?? TODO - update gradually 
    union {
        struct {
            Field flist;
        } s;
        struct {
            Vector enmconsts;
        } e;
        struct {
            constantval v; 
            Symbol s;
        } c;
        struct {
            constantval min, max;
        } limits;
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
    Symbol sym;
    Field field;
    Type type;
    _Bool lval:1;
    _Bool rval:1;
    location loc;
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



// gloabal variables

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
extern Table labels;
// TODO: types
extern Table types;
extern unsigned int level;
// pre-installed types
extern Type booltype;      // _Bool
extern Type sbooltype;     // signed _Bool
extern Type ubooltype;     // unsigned _Bool

extern Type chartype;      // char
extern Type schartype;     // signed char
extern Type uchartype;     // unsigned char

extern Type shorttype;     // signed short
extern Type sshorttype;    // signed short
extern Type ushorttype;    // unsigned short

extern Type inttype;       // signed short
extern Type sinttype;      // signed int
extern Type uinttype;      // unsigned int

extern Type longtype;      // signed long
extern Type slongtype;     // signed long
extern Type ulongtype;     // unsigned long

extern Type longlongtype;  // signed long long
extern Type slonglongtype; // signed long long
extern Type ulonglongtype; // unsigned long long

extern Type floattype;     // float
extern Type doubletype;    // double
extern Type longdoubletype;// long double

extern Type voidtype;      // void
extern Type nulltype;      // null
extern Type defaulttype;   // default

extern Type voidptype;     // void *
extern Type charptype;     // char *

extern Type uptype;        // unsigned pointer
extern Type sptype;        // signed pointer
extern Type enumtype;      // enum member

extern Node root;
extern token_store *ts;


// exported functions

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
int hash(unsigned char *str,int slen, int num_buckets); 
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
Token make_token(token tok);

// parse.c 
Node parse(char *pp_file);
void dump_AST(Node n, int indet);
void dump_cpp(char *pp_file);
char *merge_string(Token tok);

// error.c 
void warning(Location l, const char *msg);
void error(Location l, const char *msg);

// sym.c
unsigned int enterscope(); 
unsigned int exitscope(); 
Symbol install(char *name, Table *tb, unsigned int lev, int on);
Symbol lookup(char *name, Table tb);
int genlabel(int n); 



// types.c
Type make_type(int id, Type t, int size, int align, void *sym, char *strrep);
Type make_struct(int id, char *tag, Location l);
Type make_func(Type t, Vector proto);
Type make_ptr(Type t);
Type make_array(Type t, int size, int align);
Type make_dup_array(Type arr, int size, int align);
Type atop(Type t);
Type array_type(Type t);
Type deref(Type t);
Field make_field(char *name, Type t, Type ft);
int merge_qual(Type t1, Type t2);
Type qual(int id, Type t);
int isqual(Type t);
void rmtypes(int lev);
void typeinit();
void dumptypes();
char *ttos(Type t);
char *struct_to_string(Type t);
char *func_to_string(Type t);
_Bool is_basetype(Type t);
int variadic(Type t);
_Bool match_proto(Node arg_list, Type t);
_Bool conforms(Type l, Type r);
_Bool expect_noargs(Type t);
Field get_member(Type su, char *member);
Type fieldtype(Type su, Type f);
Type base_type(Type t);
_Bool is_lval(Symbol s);
_Bool modifiable_lval(Type t);
Type promote(Type t);
_Bool iscomplete(Type t);
Type usual_arithmetic_conversion(Type t1, Type t2);
_Bool iscompatible(Type t1, Type t2);
Type composite_type(Type t1, Type t2);
_Bool isvla(Type arr);
//#define isqual(t)           ((t)->id >= CONST \
                            && (t)->id <= _ATOMIC)
#define unqual(t)           (isqual(t) ? (t)->type : (t))
#define isvolatile(t)       ((t)->id == VOLATILE \
                            || (t)->id == CONST + VOLATILE)
#define isconst(t)          ((t)->id == CONST \
                            || (t)->id == CONST + VOLATILE)
#define isarray(t)          (unqual(t)->id == ARRAY)
#define isstruct(t)         (unqual(t)->id == STRUCT)
#define isunion(t)          (unqual(t)->id == UNION)
#define isfunc(t)           (unqual(t)->id == FUNCTION)
#define isptr(t)            (unqual(t)->id == POINTER)
#define isbool(t)           (unqual(t)->id == _BOOL)
#define ischar(t)           ((unqual(t)->id == SIGNED && unqual(t)->type->id == CHAR) || \
                             (unqual(t)->id == UNSIGNED && unqual(t)->type->id == CHAR) || \
                             unqual(t)->id == CHAR)
#define isint(t)            (unqual(t)->id >= CHAR \
                            && unqual(t)->id <= SIGNED \
                            || unqual(t)->id == ENUM || isbool(t))
#define isreal(t)           (isint(t) || \
                            (unqual(t)->id == FLOAT) || \
                            (unqual(t)->id == SDOUBLE) || \
                            (unqual(t)->id == LDOUBLE))
#define isarith(t)          (isreal(t))
#define isunsigned(t)       (unqual(t)->id == UNSIGNED)
#define issigned(t)         (unqual(t)->id == SIGNED)
#define isdouble(t)         ((unqual(t)->id == FLOAT) || \
                            (unqual(t)->id == SDOUBLE) || \
                            (unqual(t)->id == LDOUBLE))
#define isscalar(t)         (isarith(t) || unqual(t)->id == ENUM \
                            || unqual(t)->id == POINTER)
#define isenum(t)           (unqual(t)->id == ENUM)
#define isstructunion(t)     (unqual(t)->id == STRUCT \
                            || unqual(t)->id == UNION)

// typecheck.c
int solve_constexpr(Node const_expr);


#endif
