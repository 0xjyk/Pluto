# 1 "lex.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/stdc-predef.h" 1
# 1 "<command-line>" 2
# 1 "lex.c"
# 17 "lex.c"
# 1 "pluto.h" 1



# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/stdio.h" 1







# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/features.h" 1
# 9 "/afs/.ir/users/j/k/jk10/lib/musl/include/stdio.h" 2
# 26 "/afs/.ir/users/j/k/jk10/lib/musl/include/stdio.h"
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h" 1
# 50 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h"
typedef unsigned long size_t;
# 320 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h"
typedef struct _IO_FILE FILE;
# 331 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h"
typedef char * __isoc_va_list;
# 27 "/afs/.ir/users/j/k/jk10/lib/musl/include/stdio.h" 2
# 56 "/afs/.ir/users/j/k/jk10/lib/musl/include/stdio.h"
typedef union _G_fpos64_t {
 char __opaque[16];
 long long __lldata;
 double __align;
} fpos_t;

extern FILE *const stdin;
extern FILE *const stdout;
extern FILE *const stderr;





FILE *fopen(const char *restrict, const char *restrict);
FILE *freopen(const char *restrict, const char *restrict, FILE *restrict);
int fclose(FILE *);

int remove(const char *);
int rename(const char *, const char *);

int feof(FILE *);
int ferror(FILE *);
int fflush(FILE *);
void clearerr(FILE *);

int fseek(FILE *, long, int);
long ftell(FILE *);
void rewind(FILE *);

int fgetpos(FILE *restrict, fpos_t *restrict);
int fsetpos(FILE *, const fpos_t *);

size_t fread(void *restrict, size_t, size_t, FILE *restrict);
size_t fwrite(const void *restrict, size_t, size_t, FILE *restrict);

int fgetc(FILE *);
int getc(FILE *);
int getchar(void);
int ungetc(int, FILE *);

int fputc(int, FILE *);
int putc(int, FILE *);
int putchar(int);

char *fgets(char *restrict, int, FILE *restrict);




int fputs(const char *restrict, FILE *restrict);
int puts(const char *);

int printf(const char *restrict, ...);
int fprintf(FILE *restrict, const char *restrict, ...);
int sprintf(char *restrict, const char *restrict, ...);
int snprintf(char *restrict, size_t, const char *restrict, ...);

int vprintf(const char *restrict, __isoc_va_list);
int vfprintf(FILE *restrict, const char *restrict, __isoc_va_list);
int vsprintf(char *restrict, const char *restrict, __isoc_va_list);
int vsnprintf(char *restrict, size_t, const char *restrict, __isoc_va_list);

int scanf(const char *restrict, ...);
int fscanf(FILE *restrict, const char *restrict, ...);
int sscanf(const char *restrict, const char *restrict, ...);
int vscanf(const char *restrict, __isoc_va_list);
int vfscanf(FILE *restrict, const char *restrict, __isoc_va_list);
int vsscanf(const char *restrict, const char *restrict, __isoc_va_list);

void perror(const char *);

int setvbuf(FILE *restrict, char *restrict, int, size_t);
void setbuf(FILE *restrict, char *restrict);

char *tmpnam(char *);
FILE *tmpfile(void);
# 5 "pluto.h" 2
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/stdlib.h" 1
# 21 "/afs/.ir/users/j/k/jk10/lib/musl/include/stdlib.h"
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h" 1
# 10 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h"
typedef int wchar_t;
# 22 "/afs/.ir/users/j/k/jk10/lib/musl/include/stdlib.h" 2

int atoi (const char *);
long atol (const char *);
long long atoll (const char *);
double atof (const char *);

float strtof (const char *restrict, char **restrict);
double strtod (const char *restrict, char **restrict);
long double strtold (const char *restrict, char **restrict);

long strtol (const char *restrict, char **restrict, int);
unsigned long strtoul (const char *restrict, char **restrict, int);
long long strtoll (const char *restrict, char **restrict, int);
unsigned long long strtoull (const char *restrict, char **restrict, int);

int rand (void);
void srand (unsigned);

void *malloc (size_t);
void *calloc (size_t, size_t);
void *realloc (void *, size_t);
void free (void *);
void *aligned_alloc(size_t, size_t);

_Noreturn void abort (void);
int atexit (void (*) (void));
_Noreturn void exit (int);
_Noreturn void _Exit (int);
int at_quick_exit (void (*) (void));
_Noreturn void quick_exit (int);

char *getenv (const char *);

int system (const char *);

void *bsearch (const void *, const void *, size_t, size_t, int (*)(const void *, const void *));
void qsort (void *, size_t, size_t, int (*)(const void *, const void *));

int abs (int);
long labs (long);
long long llabs (long long);

typedef struct { int quot, rem; } div_t;
typedef struct { long quot, rem; } ldiv_t;
typedef struct { long long quot, rem; } lldiv_t;

div_t div (int, int);
ldiv_t ldiv (long, long);
lldiv_t lldiv (long long, long long);

int mblen (const char *, size_t);
int mbtowc (wchar_t *restrict, const char *restrict, size_t);
int wctomb (char *, wchar_t);
size_t mbstowcs (wchar_t *restrict, const char *restrict, size_t);
size_t wcstombs (char *restrict, const wchar_t *restrict, size_t);




size_t __ctype_get_mb_cur_max(void);
# 6 "pluto.h" 2
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/unistd.h" 1
# 37 "/afs/.ir/users/j/k/jk10/lib/musl/include/unistd.h"
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h" 1
# 65 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h"
typedef long ssize_t;




typedef long intptr_t;
# 162 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h"
typedef long off_t;
# 235 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h"
typedef int pid_t;
# 245 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h"
typedef unsigned uid_t;




typedef unsigned gid_t;
# 260 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h"
typedef unsigned useconds_t;
# 38 "/afs/.ir/users/j/k/jk10/lib/musl/include/unistd.h" 2

int pipe(int [2]);
int pipe2(int [2], int);
int close(int);
int posix_close(int, int);
int dup(int);
int dup2(int, int);
int dup3(int, int, int);
off_t lseek(int, off_t, int);
int fsync(int);
int fdatasync(int);

ssize_t read(int, void *, size_t);
ssize_t write(int, const void *, size_t);
ssize_t pread(int, void *, size_t, off_t);
ssize_t pwrite(int, const void *, size_t, off_t);

int chown(const char *, uid_t, gid_t);
int fchown(int, uid_t, gid_t);
int lchown(const char *, uid_t, gid_t);
int fchownat(int, const char *, uid_t, gid_t, int);

int link(const char *, const char *);
int linkat(int, const char *, int, const char *, int);
int symlink(const char *, const char *);
int symlinkat(const char *, int, const char *);
ssize_t readlink(const char *restrict, char *restrict, size_t);
ssize_t readlinkat(int, const char *restrict, char *restrict, size_t);
int unlink(const char *);
int unlinkat(int, const char *, int);
int rmdir(const char *);
int truncate(const char *, off_t);
int ftruncate(int, off_t);






int access(const char *, int);
int faccessat(int, const char *, int, int);

int chdir(const char *);
int fchdir(int);
char *getcwd(char *, size_t);

unsigned alarm(unsigned);
unsigned sleep(unsigned);
int pause(void);

pid_t fork(void);
pid_t _Fork(void);
int execve(const char *, char *const [], char *const []);
int execv(const char *, char *const []);
int execle(const char *, const char *, ...);
int execl(const char *, const char *, ...);
int execvp(const char *, char *const []);
int execlp(const char *, const char *, ...);
int fexecve(int, char *const [], char *const []);
_Noreturn void _exit(int);

pid_t getpid(void);
pid_t getppid(void);
pid_t getpgrp(void);
pid_t getpgid(pid_t);
int setpgid(pid_t, pid_t);
pid_t setsid(void);
pid_t getsid(pid_t);
char *ttyname(int);
int ttyname_r(int, char *, size_t);
int isatty(int);
pid_t tcgetpgrp(int);
int tcsetpgrp(int, pid_t);

uid_t getuid(void);
uid_t geteuid(void);
gid_t getgid(void);
gid_t getegid(void);
int getgroups(int, gid_t []);
int setuid(uid_t);
int seteuid(uid_t);
int setgid(gid_t);
int setegid(gid_t);

char *getlogin(void);
int getlogin_r(char *, size_t);
int gethostname(char *, size_t);
char *ctermid(char *);

int getopt(int, char * const [], const char *);
extern char *optarg;
extern int optind, opterr, optopt;

long pathconf(const char *, int);
long fpathconf(int, int);
long sysconf(int);
size_t confstr(int, char *, size_t);
# 7 "pluto.h" 2
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/assert.h" 1
# 19 "/afs/.ir/users/j/k/jk10/lib/musl/include/assert.h"
_Noreturn void __assert_fail (const char *, const char *, int, const char *);
# 8 "pluto.h" 2
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/stddef.h" 1
# 19 "/afs/.ir/users/j/k/jk10/lib/musl/include/stddef.h"
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h" 1
# 41 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h"
typedef struct { long long __ll; long double __ld; } max_align_t;
# 60 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h"
typedef long ptrdiff_t;
# 20 "/afs/.ir/users/j/k/jk10/lib/musl/include/stddef.h" 2
# 9 "pluto.h" 2
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/uchar.h" 1
# 9 "/afs/.ir/users/j/k/jk10/lib/musl/include/uchar.h"
typedef unsigned short char16_t;
typedef unsigned char32_t;






# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h" 1
# 337 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h"
typedef struct __mbstate_t { unsigned __opaque1, __opaque2; } mbstate_t;
# 18 "/afs/.ir/users/j/k/jk10/lib/musl/include/uchar.h" 2

size_t c16rtomb(char *restrict, char16_t, mbstate_t *restrict);
size_t mbrtoc16(char16_t *restrict, const char *restrict, size_t, mbstate_t *restrict);

size_t c32rtomb(char *restrict, char32_t, mbstate_t *restrict);
size_t mbrtoc32(char32_t *restrict, const char *restrict, size_t, mbstate_t *restrict);
# 10 "pluto.h" 2
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/libgen.h" 1







char *dirname(char *);
char *basename(char *);
# 11 "pluto.h" 2
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/string.h" 1
# 25 "/afs/.ir/users/j/k/jk10/lib/musl/include/string.h"
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h" 1
# 26 "/afs/.ir/users/j/k/jk10/lib/musl/include/string.h" 2

void *memcpy (void *restrict, const void *restrict, size_t);
void *memmove (void *, const void *, size_t);
void *memset (void *, int, size_t);
int memcmp (const void *, const void *, size_t);
void *memchr (const void *, int, size_t);

char *strcpy (char *restrict, const char *restrict);
char *strncpy (char *restrict, const char *restrict, size_t);

char *strcat (char *restrict, const char *restrict);
char *strncat (char *restrict, const char *restrict, size_t);

int strcmp (const char *, const char *);
int strncmp (const char *, const char *, size_t);

int strcoll (const char *, const char *);
size_t strxfrm (char *restrict, const char *restrict, size_t);

char *strchr (const char *, int);
char *strrchr (const char *, int);

size_t strcspn (const char *, const char *);
size_t strspn (const char *, const char *);
char *strpbrk (const char *, const char *);
char *strstr (const char *, const char *);
char *strtok (char *restrict, const char *restrict);

size_t strlen (const char *);

char *strerror (int);
# 12 "pluto.h" 2
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/limits.h" 1





# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h" 1
# 7 "/afs/.ir/users/j/k/jk10/lib/musl/include/limits.h" 2
# 13 "pluto.h" 2
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/fcntl.h" 1
# 20 "/afs/.ir/users/j/k/jk10/lib/musl/include/fcntl.h"
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h" 1
# 152 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h"
typedef unsigned mode_t;
# 21 "/afs/.ir/users/j/k/jk10/lib/musl/include/fcntl.h" 2

# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/fcntl.h" 1
# 23 "/afs/.ir/users/j/k/jk10/lib/musl/include/fcntl.h" 2

struct flock {
 short l_type;
 short l_whence;
 off_t l_start;
 off_t l_len;
 pid_t l_pid;
};

int creat(const char *, mode_t);
int fcntl(int, int, ...);
int open(const char *, int, ...);
int openat(int, const char *, int, ...);
int posix_fadvise(int, off_t, off_t, int);
int posix_fallocate(int, off_t, off_t);
# 14 "pluto.h" 2
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/sys/wait.h" 1
# 11 "/afs/.ir/users/j/k/jk10/lib/musl/include/sys/wait.h"
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h" 1
# 240 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h"
typedef unsigned id_t;
# 12 "/afs/.ir/users/j/k/jk10/lib/musl/include/sys/wait.h" 2

typedef enum {
 P_ALL = 0,
 P_PID = 1,
 P_PGID = 2,
 P_PIDFD = 3
} idtype_t;

pid_t wait (int *);
pid_t waitpid (pid_t, int *, int );
# 15 "pluto.h" 2
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/errno.h" 1
# 10 "/afs/.ir/users/j/k/jk10/lib/musl/include/errno.h"
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/errno.h" 1
# 11 "/afs/.ir/users/j/k/jk10/lib/musl/include/errno.h" 2




int *__errno_location(void);
# 16 "pluto.h" 2
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/float.h" 1







int __flt_rounds(void);
# 46 "/afs/.ir/users/j/k/jk10/lib/musl/include/float.h"
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/float.h" 1
# 47 "/afs/.ir/users/j/k/jk10/lib/musl/include/float.h" 2
# 17 "pluto.h" 2
# 39 "pluto.h"
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

    char nf:1;
    char rtf:1;
    char shf:1;
    char tae:1;
} location;
typedef location *Location;

typedef struct buffer {
    char *file;
    int fd;
    char buf[4096 + 4096 + 1];
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

        } charval;
        char *strval;
    } val;

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


# 1 "include/token.h" 1
LSQBRACREP=91,
RSQBRACREP=93,
LCBRACREP=123,
RCBRACREP=125,
HASHREP=35,
HHASHREP=255,

ID=0,
KEYWORD=1,
PUNCT=2,
ERROR=3,
ULLONG=4,
SLLONG=5,
ULONG=6,
SLONG=7,
UINT=8,
SINT=9,
SDOUBLE=10,
LDOUBLE=11,
SFLOAT=12,
UCHAR=13,
SCHAR=14,
CHAR16=15,
CHAR32=16,
STR=17,
INTCONST=18,
FLOATCONST=19,
NOT=33,
HASH=35,
MOD=37,
BAND=38,
LBRAC=40,
RBRAC=41,
STAR=42,
PLUS=43,
COM=44,
NEG=45,
DOT=46,
DIV=47,
COL=58,
SCOL=59,
LESS=60,
ASSIGN=61,
GREATER=62,
Q=63,
LSQBRAC=91,
RSQBRAC=93,
XOR=94,
LCBRAC=123,
BOR=124,
RCBRAC=125,
BNOT=126,



ARROW=233,
INCR=234,
DECR=235,
LSHFT=236,
RSHFT=237,
LEQ=238,
GEQ=239,
EQ=240,
NEQ=241,
AND=242,
OR=243,
ELLIPS=244,
MULTEQ=245,
DIVEQ=246,
MODEQ=247,
INCREQ=248,
DECREQ=249,
LSHFTEQ=250,
RSHFTEQ=251,
ANDEQ=252,
XOREQ=253,
OREQ=254,
HHASH=255,
EOI=256,


BREAK=128,
CASE=129,
CONTINUE=130,
DEFAULT=131,
DO=132,
ELSE=133,
FOR=134,
GOTO=135,
IF=136,
RETURN=137,
SIZEOF=138,
SWITCH=139,
WHILE=140,
_ALIGNOF=141,
_GENERIC=142,
_IMAGINARY=143,
_STATIC_ASSERT=144,




CHAR=145,
SHORT=146,
INT=147,
LONG=148,
UNSIGNED=149,
SIGNED=150,
FLOAT=151,
DOUBLE=152,

STRUCT=153,
UNION=154,
ENUM=155,

VOID=156,

FUNCTION=157,
ARRAY=158,
POINTER=159,
_BOOL=160,
_COMPLEX=161,


CONST=162,
VOLATILE=163,
RESTRICT=164,
_ATOMIC=165,


TYPEDEF=166,
EXTERN=167,
STATIC=168,
_THREAD_LOCAL=169,
AUTO=170,
REGISTER=171,


_ALIGNAS=172,


INLINE=173,
_NORETURN=174,


NULLT=174,
# 138 "pluto.h" 2


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

    struct node *arr_size;
    union {

        Symbol sym;

        struct {

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

    Table prev;

    struct entry {
        symbol sym;
        struct entry *next;
    } *buckets[256];

    Symbol all;
} table;

enum {

# 1 "include/node.h" 1
ND_ID=256,
ND_STR=257,
ND_UCHAR=258,
ND_INT=259,
ND_FLOAT=260,
ND_CONST=261,
ND_EXPR=262,
ND_GENERIC=263,
ND_PRIMARY=267,


ND_SELECT=268,
ND_CALL=269,
ND_DOT=270,
ND_ARROW=271,
ND_INCR=272,
ND_DECR=273,
ND_TYP_INIT=274,
ND_POSTFIX=275,


ND_UINCR=276,
ND_UDECR=277,
ND_UAND=278,
ND_UDEREF=279,
ND_UPLUS=280,
ND_UMINUS=281,
ND_UBITNOT=282,
ND_UNOT=283,
ND_UNARY=284,
ND_ALIGNOF=285,

ND_CAST=286,
ND_MULT=287,
ND_DIV=288,
ND_MOD=289,
ND_ADD=290,
ND_SUB=291,
ND_LSHFT=292,
ND_RSHFT=293,
ND_LESS=294,
ND_GREATER=295,
ND_LEQ=296,
ND_GEQ=297,
ND_EQ=298,
ND_NEQ=299,
ND_BAND=300,
ND_XOR=301,
ND_IOR=302,
ND_AND=303,
ND_OR=304,
ND_COND=305,

ND_ASSIGN=306,
ND_MULT_ASSIGN=307,
ND_DIV_ASSIGN=308,
ND_MOD_ASSIGN=309,
ND_INCR_ASSIGN=310,
ND_DECR_ASSIGN=311,
ND_LSHFT_ASSIGN=312,
ND_RSHFT_ASSIGN=313,
ND_AND_ASSIGN=314,
ND_XOR_ASSIGN=315,
ND_OR_ASSIGN=316,
ND_ASSIGNEXPR=317,
ND_ARG_LIST=318,

ND_DS=319,
ND_TYPE_DECL=320,
ND_DECL=321,
ND_DECL_LIST=322,
ND_FUNC_DEF=323,
ND_FUNC_DECL=324,
ND_PARM_LIST=325,
ND_PARM_LIST_VARD=326,
ND_PARM=327,
ND_INIT=328,
ND_INIT_LIST=329,
ND_DESIG_LIST=330,
ND_SAD=331,
ND_ABS_DECL=332,
ND_SQL=333,
ND_ROOT=400,


ND_GENLS=350,
ND_GEN_ASSOC=351,
ND_DEFAULT=352,
ND_TYPENAME=353,
ND_COMP_LIT=354,
ND_SIZEOF=355,

ND_ID_LABEL=356,
ND_CASE_LABEL=357,
ND_DEFAULT_LABEL=358,
ND_IF_STMT=359,
ND_SWITCH_STMT=360,
ND_WHILE=361,
ND_DO_WHILE=362,
ND_FOR=363,
ND_GOTO=364,
ND_CONTINUE=365,
ND_BREAK=366,
ND_RETURN=367,
ND_COMPOUND=368,
# 241 "pluto.h" 2

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

        } charval;
        char *strval;
    } val;

} node;





extern unsigned char JOBSCOPE;
extern int num_files;
extern char **infiles;
static char *outfile = ((void*)0);
static char *cwd = ((void*)0);
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

extern Table types;
extern unsigned int level;

extern Type booltype;
extern Type sbooltype;
extern Type ubooltype;

extern Type chartype;
extern Type schartype;
extern Type uchartype;

extern Type shorttype;
extern Type sshorttype;
extern Type ushorttype;

extern Type inttype;
extern Type sinttype;
extern Type uinttype;

extern Type longtype;
extern Type slongtype;
extern Type ulongtype;

extern Type longlongtype;
extern Type slonglongtype;
extern Type ulonglongtype;

extern Type floattype;
extern Type doubletype;
extern Type longdoubletype;

extern Type voidtype;
extern Type nulltype;
extern Type defaulttype;

extern Type voidptype;
extern Type charptype;

extern Type uptype;
extern Type sptype;

extern Node root;
extern token_store *ts;





void parseoptions(int argc, char **argv);
void usage();
void preprocess();
void process();
void remove_tempfiles();


Vector vec_init(size_t size);
void vec_free(Vector vec);
void vec_resize(Vector vec, size_t size);
void vec_pushback(Vector vec, void *ptr);
void *vec_get(Vector vec, size_t i);
void *vec_put(Vector vec, size_t i, void *ptr);


void *alloc(unsigned long sz, int an);
void dealloc(int an);
char *strloc(char *str, int sz, int an);


int hash(unsigned char *str,int slen, int num_buckets);
char *make_string(char *str, int len);
char *dtos(int n);
void dump_stringpool();


void fillbuf();
void bufinit(char *pp_file);
void ensure_buflen(int len);


Token lex();
void lexdriver(char *pp_file);
Token make_token(token tok);


Node parse(char *pp_file);
void dump_AST(Node n, int indet);
void dump_cpp(char *pp_file);
char *merge_string(Token tok);


void warning(Location l, const char *msg);
void error(Location l, const char *msg);


unsigned int enterscope();
unsigned int exitscope();
Symbol install(char *name, Table *tb, unsigned int lev, int on);
Symbol lookup(char *name, Table tb);
int genlabel(int n);




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
# 483 "pluto.h"
int solve_constexpr(Node const_expr);
# 18 "lex.c" 2
# 1 "include/charmap.h" 1
enum { BLANK=01, NEWLINE=02, LETTER=04,
       DIGIT=010, HEX=020, OTHER=040 };

static unsigned char map[256] = { 0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     BLANK,
                     NEWLINE,
                     BLANK,
                     BLANK,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     BLANK,
                     OTHER,
                     OTHER,
                     OTHER,
                     0,
                     OTHER,
                     OTHER,
                     OTHER,
                     OTHER,
                     OTHER,
                     OTHER,
                     OTHER,
                     OTHER,
                     OTHER,
                     OTHER,
                     OTHER,
                     DIGIT,
                     DIGIT,
                     DIGIT,
                     DIGIT,
                     DIGIT,
                     DIGIT,
                     DIGIT,
                     DIGIT,
                     DIGIT,
                     DIGIT,
                     OTHER,
                     OTHER,
                     OTHER,
                     OTHER,
                     OTHER,
                     OTHER,
                     0,
                     LETTER|HEX,
                     LETTER|HEX,
                     LETTER|HEX,
                     LETTER|HEX,
                     LETTER|HEX,
                     LETTER|HEX,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     OTHER,
                     OTHER,
                     OTHER,
                     OTHER,
                     LETTER,
                     0,
                     LETTER|HEX,
                     LETTER|HEX,
                     LETTER|HEX,
                     LETTER|HEX,
                     LETTER|HEX,
                     LETTER|HEX,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     LETTER,
                     OTHER,
                     OTHER,
                     OTHER,
                     OTHER, };
# 19 "lex.c" 2


Token is_identifier();
Token is_constant();
Token is_intconst();
Token is_floatconst();
Token is_enumconst();
Token is_charconst();
Token is_string();
Token is_keyword();
Token is_punct();
int is_escapeseq(int *val, char *s);
void process_gnu_directive();





Token lex() {




    if (ts) {
        Token tok = ts->tok;
        ts = ts->nxt;
        return tok;
    }

    if ((buf.bufend - buf.curr) <= 32 && !pp_read_complete) {
        fillbuf();
    }
    if (pp_read_complete && (buf.bufend - 1 - buf.curr <= 0)) {
        token tok = {.type=EOI, .len=0, .loc=loc};
        return make_token(tok);
    }



    while ((*buf.curr == '#') ||
            (map[*buf.curr] & BLANK) ||
            (map[*buf.curr] & NEWLINE)) {
        if (*buf.curr == '#') {
            process_gnu_directive();

        } else if (map[*buf.curr] & BLANK) {
            buf.curr++;
            loc.x++;

        } else if (map[*buf.curr] & NEWLINE) {
            buf.curr++;
            loc.x = 1;
            loc.y++;
        }
    }
    if (pp_read_complete && (buf.bufend - 1 - buf.curr <= 0)) {
        token tok = {.type=EOI, .len=0, .loc=loc};
        return make_token(tok);
    }
    Token t = ((void*)0);


    if ((*buf.curr == 'u') || (*buf.curr == 'U') || (*buf.curr == 'L')) {
        char *s = buf.curr;
        if (buf.curr[1] == '\'') {

            s += 2;
            while (*s != '\'') {
                s++;
            }
            s++;
            error(&loc, "pluto doesn't support char16_t, char32_t & wchar_t character constants");
            token tok = {.type=ERROR, .subtype=UCHAR, .len=(s-buf.curr), .loc=loc};
            tok.val.strval = make_string(buf.curr, tok.len);
            return make_token(tok);
        } else if (buf.curr[1] == '\"') {

            ensure_buflen((512 + 3));
            s = buf.curr + 2;
            while (*s != '\"') {
                s++;
            }
            s++;
            error(&loc, "pluto doesn't support char16_t, char32_t & wchar_t string constants");
            token tok = {.type=ERROR, .subtype=STR, .len=(s-buf.curr), .loc=loc};
            tok.val.strval = make_string(buf.curr, tok.len);
            return make_token(tok);
        } else if ((buf.curr[0] == 'u') && (buf.curr[1] == '8')
                && (buf.curr[2] == '\"')) {

            ensure_buflen((512 + 3));
            s = buf.curr + 3;
            while (*s != '\"') {
                s++;
            }
            s++;
            error(&loc, "pluto doesn't support UTF-8 string constants");
            token tok = {.type=ERROR, .subtype=STR, .len=(s-buf.curr), .loc=loc};
            tok.val.strval = make_string(buf.curr, tok.len);
            return make_token(tok);
        }
    }
    if (*buf.curr == '\"' && (t = is_string())) {
        return t;
    } else if ((map[*buf.curr] & LETTER) && (t = is_keyword())) {
        return t;
    } else if ((map[*buf.curr] & LETTER) && (t = is_identifier())) {
        return t;
    } else if (t = is_punct()) {
        return t;
    } else if (t = is_constant()) {
        return t;

    } else {
        error(&loc, "detected an unknown character");

        buf.curr++;
        return lex();
    }

}


void lexdriver(char *pp_file) {

    bufinit(pp_file);


    char lex_file[PATH_MAX] = "";
    strcat(lex_file, pp_file);
    lex_file[strlen(lex_file)-2] = 'l';
    lex_file[strlen(lex_file)-1] = 'l';
    FILE *fp = fopen(lex_file, "w");
    if (fp == ((void*)0)) {
        printf("couldn't open %s\n", lex_file);
        exit(1);
    }

    char *tok_map[256];


# 1 "include/token.h" 1
tok_map[91] = "<:";
tok_map[93] = ":>";
tok_map[123] = "<%";
tok_map[125] = "%>";
tok_map[35] = "%:";
tok_map[255] = "%:%:";

tok_map[0] = "identifier";
tok_map[1] = "keyword";
tok_map[2] = "punctuation";
tok_map[3] = "error";
tok_map[4] = "unsigned long long int";
tok_map[5] = "long long int";
tok_map[6] = "unsigned long int";
tok_map[7] = "long int";
tok_map[8] = "unsigned int";
tok_map[9] = "int";
tok_map[10] = "double";
tok_map[11] = "long double";
tok_map[12] = "float";
tok_map[13] = "unsigned char";
tok_map[14] = "signed char";
tok_map[15] = "char 16";
tok_map[16] = "char 32";
tok_map[17] = "string";
tok_map[18] = "int const";
tok_map[19] = "float const";
tok_map[33] = "!";
tok_map[35] = "#";
tok_map[37] = "%";
tok_map[38] = "&";
tok_map[40] = "(";
tok_map[41] = ")";
tok_map[42] = "*";
tok_map[43] = "+";
tok_map[44] = ",";
tok_map[45] = "-";
tok_map[46] = ".";
tok_map[47] = "/";
tok_map[58] = ":";
tok_map[59] = ";";
tok_map[60] = "<";
tok_map[61] = "=";
tok_map[62] = ">";
tok_map[63] = "?";
tok_map[91] = "[";
tok_map[93] = "]";
tok_map[94] = "^";
tok_map[123] = "{";
tok_map[124] = "|";
tok_map[125] = "}";
tok_map[126] = "~";



tok_map[233] = "->";
tok_map[234] = "++";
tok_map[235] = "--";
tok_map[236] = "<<";
tok_map[237] = ">>";
tok_map[238] = "<=";
tok_map[239] = ">=";
tok_map[240] = "==";
tok_map[241] = "!=";
tok_map[242] = "&&";
tok_map[243] = "||";
tok_map[244] = "...";
tok_map[245] = "*=";
tok_map[246] = "/=";
tok_map[247] = "%=";
tok_map[248] = "+=";
tok_map[249] = "-=";
tok_map[250] = "<<=";
tok_map[251] = ">>=";
tok_map[252] = "&=";
tok_map[253] = "^=";
tok_map[254] = "|=";
tok_map[255] = "##";
tok_map[256] = "end of input";


tok_map[128] = "break";
tok_map[129] = "case";
tok_map[130] = "continute";
tok_map[131] = "default";
tok_map[132] = "do";
tok_map[133] = "else";
tok_map[134] = "for";
tok_map[135] = "goto";
tok_map[136] = "if";
tok_map[137] = "return";
tok_map[138] = "sizeof";
tok_map[139] = "switch";
tok_map[140] = "while";
tok_map[141] = "_Alignof";
tok_map[142] = "_Generic";
tok_map[143] = "_Imaginary";
tok_map[144] = "_Static_assert";




tok_map[145] = "char";
tok_map[146] = "short";
tok_map[147] = "int";
tok_map[148] = "long";
tok_map[149] = "unsigned";
tok_map[150] = "signed";
tok_map[151] = "float";
tok_map[152] = "double";

tok_map[153] = "struct";
tok_map[154] = "union";
tok_map[155] = "enum";

tok_map[156] = "void";

tok_map[157] = "function";
tok_map[158] = "array";
tok_map[159] = "pointer";
tok_map[160] = "_Bool";
tok_map[161] = "_Complex";


tok_map[162] = "const";
tok_map[163] = "volatile";
tok_map[164] = "restrict";
tok_map[165] = "_Atomic";


tok_map[166] = "typedef";
tok_map[167] = "extern";
tok_map[168] = "static";
tok_map[169] = "_Thread_local";
tok_map[170] = "auto";
tok_map[171] = "register";


tok_map[172] = "_Alignas";


tok_map[173] = "inline";
tok_map[174] = "_Noreturn";


tok_map[174] = "null";
# 161 "lex.c" 2



    Token tok = ((void*)0);
    while (1) {
        tok = lex();
        if (tok == ((void*)0)) {
            fprintf(fp, "INTERNAL ERROR!\n");
            fprintf(fp, "location: %d, %d\n", tok->loc.x, tok->loc.y);
            continue;
        }
        switch (tok->type) {
            case ID:
                fprintf(fp, "ID \"%s\"\n", tok->val.strval); break;
            case KEYWORD:
                fprintf(fp, "KEYWORD: %s \"%s\"\n", tok_map[tok->subtype], tok->val.strval); break;
            case PUNCT:
                fprintf(fp, "PUNCT: %s \"%s\"\n", tok_map[tok->subtype], tok->val.strval); break;
            case UCHAR:
                fprintf(fp, "UCHAR \'%c\'\n", tok->val.charval.c); break;
            case STR:
                fprintf(fp, "STR \"%s\"\n", tok->val.strval); break;
            case INTCONST:
                switch (tok->subtype) {
                    case ULLONG:
                        fprintf(fp, "INTCONST: %s %llu\n", tok_map[tok->subtype], tok->val.intval.ull);
                        break;
                    case SLLONG:
                        fprintf(fp, "INTCONST: %s %lld\n", tok_map[tok->subtype], tok->val.intval.ll);
                        break;
                    case ULONG:
                        fprintf(fp, "INTCONST: %s %lu\n", tok_map[tok->subtype], tok->val.intval.ul);
                        break;
                    case SLONG:
                        fprintf(fp, "INTCONST: %s %ld\n", tok_map[tok->subtype], tok->val.intval.l);
                        break;
                    case UINT:
                        fprintf(fp, "INTCONST: %s %u\n", tok_map[tok->subtype], tok->val.intval.u);
                        break;
                    case SINT:
                        fprintf(fp, "INTCONST: %s %d\n", tok_map[tok->subtype], tok->val.intval.i);
                        break;
                }
                break;
            case FLOATCONST:
                switch (tok->subtype) {
                    case SDOUBLE:
                        fprintf(fp, "FLOATCONST: %s %f\n", tok_map[tok->subtype], tok->val.floatval.d);
                        break;
                    case LDOUBLE:
                        fprintf(fp, "FLOATCONST: %s %Lf\n", tok_map[tok->subtype], tok->val.floatval.ld);
                        break;
                    case SFLOAT:
                        fprintf(fp, "FLOATCONST: %s %f\n", tok_map[tok->subtype], tok->val.floatval.f);
                        break;
                }
                break;
            case ERROR:
                fprintf(fp, "ERROR \"%s\"\n", tok->val.strval); break;
            case EOI:
                fprintf(fp, "EOI\n");
                fprintf(fp, "location: %d, %d\n", tok->loc.x, tok->loc.y); return;
            default:
                fprintf(fp, "INTERNAL ERROR: lex driver shouldn't print this message\n");
                return;
        }
        fprintf(fp, "location: %d, %d\n", tok->loc.x, tok->loc.y);
    }
}


Token is_identifier() {
    ensure_buflen(32);
    char *s = buf.curr;


    while (map[*s] & (DIGIT | LETTER)) {
        s++;

        if (pp_read_complete && (buf.bufend - 1 - s <= 0)) {
            break;
        }
    }

    token tok = {.type=ID, .len=(s-buf.curr), .loc=loc};
    tok.val.strval = make_string(buf.curr, (s - buf.curr));
    Token t = make_token(tok);
    return t;
}

Token is_constant() {
    Token t = ((void*)0);
    if ((t = is_floatconst()) ||
            (t = is_intconst()) ||
            (t = is_charconst())) {
        return t;
    }
    return ((void*)0);
}


Token is_intconst() {
    char *s = buf.curr;
    int err = 0;
    char unsigned_flag = 0;
    char long_flag = 0;
    char llong_flag = 0;


    unsigned long long int value = 0;



    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        s += 2;
        while((map[s[0]] & DIGIT) || (map[s[0]] & HEX)) {
            value <<= 4;
            if (map[s[0]] & DIGIT) {
                value += (s[0] - 48);
            } else if (s[0] <= 70) {
                value += (s[0] - 55);
            } else {
                value += (s[0] - 87);
            }
            s++;
        }
        if (s == (buf.curr + 2)) {

            err = 1;
            error(&loc, "hexadecimal integer constant requires hex digits after 0x");
            token tok = {.type=ERROR, .subtype=SINT, .len=(s-buf.curr), .loc=loc};
            tok.val.strval = make_string(buf.curr, (s - buf.curr));
            return make_token(tok);
        }
    } else if (s[0] == '0') {

        while ((map[s[0]] & DIGIT) && (s[0] <= 55)) {
            value *= 8;
            value += (s[0] - 48);
            s++;
        }


    } else if ((map[s[0]] & DIGIT) && s[0] != '0') {
        while (map[s[0]] & DIGIT) {
            value *= 10;
            value += (s[0] - 48);
            s++;
        }
    } else {
        return ((void*)0);
    }


    if (s[0] == 'u' || s[0] == 'U') {
        unsigned_flag = 1;
        s++;
        if (s[0] == 'l' || s[0] == 'L') {
            long_flag = 1;
            s++;
            if (s[0] == 'l' || s[0] == 'L') {
                llong_flag = long_flag--;
                s++;
            }
        }
    } else if (s[0] == 'l' || s[0] == 'L') {
        long_flag = 1;
        s++;
        if (s[0] == 'l' || s[0] == 'L') {
            llong_flag = long_flag--;
            s++;
            if (s[0] == 'u' || s[0] == 'U') {
                unsigned_flag = 1;
                s++;
            }
        }
    }
    token tok = {.type=INTCONST, .len=(s-buf.curr), .loc=loc};




    if (unsigned_flag && llong_flag) {

        tok.subtype = ULLONG;
        tok.val.intval.ull = value;
        if (value > (2ULL*0x7fffffffffffffffLL +1)) {
            tok.type = ERROR;
            tok.subtype = ULLONG;
            tok.val.strval = make_string(buf.curr, s - buf.curr);
            error(&loc, "int const (unsigned long long) larger than ULLONG_MAX");
        }
    } else if (llong_flag) {
        tok.subtype = SLLONG;
        tok.val.intval.ll = (long long int) value;
        if (value > 0x7fffffffffffffffLL) {
            tok.type = ERROR;
            tok.subtype = SLLONG;
            tok.val.strval = make_string(buf.curr, s - buf.curr);
            error(&loc, "int const (signed long long) larger than LLONG_MAX");
        }
    } else if (unsigned_flag && long_flag) {
        tok.subtype = ULONG;
        tok.val.intval.ul = (unsigned long int) value;
        if (value > (2UL*0x7fffffffffffffffL +1)) {
            tok.subtype = ULLONG;
            tok.val.intval.ull = value;
            if (value > (2ULL*0x7fffffffffffffffLL +1)) {

                tok.type = ERROR;
                tok.subtype = ULLONG;
                tok.val.strval = make_string(buf.curr, s - buf.curr);
                error(&loc, "int const (unsigned long long) larger than ULLONG_MAX");
            }
        }
    } else if (long_flag) {
        tok.subtype = SLONG;
        tok.val.intval.l = (long int) value;
        if (value > 0x7fffffffffffffffL) {
            tok.subtype = SLLONG;
            tok.val.intval.ll = (long long int) value;
            if (value > 0x7fffffffffffffffLL) {

                tok.type = ERROR;
                tok.subtype = SLLONG;
                tok.val.strval = make_string(buf.curr, s - buf.curr);
                error(&loc, "int const (signed long long) larger than LLONG_MAX");
            }
        }
    } else if (unsigned_flag) {
        tok.subtype = UINT;
        tok.val.intval.u = (unsigned int) value;
        if (value > 0xffffffffU) {
            tok.subtype = ULONG;
            tok.val.intval.ul = (unsigned long int) value;
            if (value > (2UL*0x7fffffffffffffffL +1)) {
                tok.subtype = ULLONG;
                tok.val.intval.ull = (unsigned long long int) value;
                if (value > (2ULL*0x7fffffffffffffffLL +1)) {

                    tok.type = ERROR;
                    tok.subtype = ULLONG;
                    tok.val.strval = make_string(buf.curr, s - buf.curr);
                    error(&loc, "int const (unsigned long long) larger than ULLONG_MAX");
                }
            }
        }
    } else {
        tok.subtype = SINT;
        tok.val.intval.i = (int) value;
        if (value > 0x7fffffff) {
            tok.subtype = SLONG;
            tok.val.intval.l = (long int) value;
            if (value > 0x7fffffffffffffffL) {
                tok.subtype = SLLONG;
                tok.val.intval.ll = (long long int) value;
                if (value > 0x7fffffffffffffffLL) {

                    tok.type = ERROR;
                    tok.subtype = SLLONG;
                    tok.val.strval = make_string(buf.curr, s - buf.curr);
                    error(&loc, "int const (signed long long) larger than LLONG_MAX");
                }
            }
        }

    }
    return make_token(tok);
}

Token is_floatconst() {
    char *s = buf.curr;

    char double_flag = 1;

    char float_flag = 0;
    char longdouble_flag = 0;
    char err = 0;



    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        s += 2;
        while (map[s[0]] & (HEX | DIGIT)) {
            s++;
        }
        if (s[0] == '.') {
            s++;
            while (map[s[0]] & (HEX | DIGIT)) {
                s++;
            }
            if (s[0] == 'p' || s[0] == 'P') {
                s++;
                if (s[0] == '+' || s[0] == '-') {
                    s++;
                }
                if (map[s[0]] & DIGIT) {
                    while (map[s[0]] & DIGIT) {
                        s++;
                    }
                } else {

                    err = 1;
                    error(&loc, "hex/octal float const expected digit after p/P");
                }
            }
        } else if (s[0] == 'p' || s[0] == 'P') {
            s++;
            if (s[0] == '+' || s[0] == '-') {
                s++;
            }
            if (map[s[0]] & DIGIT) {
                while (map[s[0]] & DIGIT) {
                     s++;
                }
            } else {

                err = 1;
                error(&loc, "hex/octal float const expected digit after p/P");
             }

        } else {

            return ((void*)0);
        }





    } else if (map[s[0]] & DIGIT || s[0] == '.') {
        while (map[s[0]] & DIGIT) {
            s++;
        }
        if (s[0] == '.') {
            s++;
            while (map[s[0]] & DIGIT) {
                s++;
            }
            if (s[0] == 'e' || s[0] == 'E') {
                s++;
                if (s[0] == '+' || s[0] == '-') {
                    s++;
                }
                if (map[s[0]] & DIGIT) {
                    while (map[s[0]] & DIGIT) {
                        s++;
                    }
                } else {

                    err = 1;
                    error(&loc, "hex/octal float const expected digit after e/E");
                }
            }
        } else if (s[0] == 'e' || s[0] == 'E') {
            s++;
            if (s[0] == '+' || s[0] == '-') {
                s++;
            }
            if (map[s[0]] & DIGIT) {
                while (map[s[0]] & DIGIT) {
                     s++;
                }
            } else {

                err = 1;
                error(&loc, "hex/octal float const expected digit after e/E");
             }

        } else {

            return ((void*)0);
        }





    } else {
        return ((void*)0);
    }

    if (s[0] == 'f' || s[0] == 'F') {
        double_flag = 0;
        float_flag = 1;
        s++;
    } else if (s[0] == 'l' || s[0] == 'L') {
        double_flag = 0;
        longdouble_flag = 1;
        s++;
    }
    if (err) {
        token tok = {.type=ERROR, .subtype=SFLOAT, .len=(s-buf.curr), .loc=loc};
        tok.val.strval = make_string(buf.curr, (s - buf.curr));
        return make_token(tok);
    }

    token tok = {.type=FLOATCONST, .loc=loc};
    if (double_flag) {
        tok.subtype = SDOUBLE;
        tok.val.floatval.d = strtod(buf.curr, &s);

        if ((*__errno_location()) == 34) {
            (*__errno_location()) = 0;
            err = 1;
            error(&loc, "float const (double) exceeds max limit");
        }
    } else if (longdouble_flag) {
        tok.subtype = LDOUBLE;
        tok.val.floatval.ld = strtold(buf.curr, &s);
        s++;

        if ((*__errno_location()) == 34) {
            (*__errno_location()) = 0;

            err = 1;
            error(&loc, "float const (long double) exceeds max limit");
        }
    } else if (float_flag) {
        tok.subtype = SFLOAT;
        tok.val.floatval.f = strtof(buf.curr, &s);
        s++;

        if ((*__errno_location()) == 34) {
            (*__errno_location()) = 0;

            err = 1;
            error(&loc, "float const (signed float) exceeds max limit");
        }
    }
    if (err) {

        token tok = {.type=ERROR, .subtype=SFLOAT, .len=(s-buf.curr), .loc=loc};
        tok.val.strval = make_string(buf.curr, (s - buf.curr));
        return make_token(tok);
    }
    tok.len = s - buf.curr;
    return make_token(tok);
}


Token is_charconst() {
    if (*buf.curr != '\'') {
        return ((void*)0);
    }
    ensure_buflen(32);
    char *s = buf.curr + 1;
    int val = 0;
    char err = 0;

    if (*s == '\\') {
        int esc_len = is_escapeseq(&val, s);
        if (!esc_len) {

            err = 1;
            error(&loc, "escape sequence requires atleast one valid escape character after \\");
            val = 0;
            esc_len = 1;
        }
        s += esc_len;
    } else {
        val = *s++;
    }
    if (*s++ != '\'') {
        err = 1;
        error(&loc, "In it's current implementation, pluto only supports single character constants");


        while (*s != '\'') {
            s++;
            ensure_buflen(32);
            if (s == (buf.bufend - 1)) {
                err = 1;
                error(&loc, "unterminated character constant");
            }
        }
    }
    if (err) {
        token tok = {.type=ERROR, .subtype=UCHAR, .len=(s-buf.curr), .loc=loc};
        tok.val.strval = make_string(buf.curr, (s - buf.curr));
        return make_token(tok);
    }
    token tok = {.type=UCHAR, .len=(s-buf.curr), .loc=loc};
    tok.val.charval.c = val;
    return make_token(tok);
}

int is_escapeseq(int *val, char *s) {
    *val = 0;
    char *olds = s;

    if (s[0] == '\\') {
        switch (s[1]) {
            case '\'':
                *val = '\'';
                return 2;
            case '\"':
                *val = '\"';
                return 2;
            case '?':
                *val = '\?';
                return 2;
            case '\\':
                *val = '\\';
                return 2;
            case 'v':
                *val = '\v';
                return 2;
            case 'f':
                *val = '\f';
                return 2;
            case 'r':
                *val = '\r';
                return 2;
            case 't':
                *val = '\t';
                return 2;
            case 'n': case '\n':
                *val = '\n';
                return 2;
            case 'a':
                *val = '\a';
                return 2;
            case 'b':
                *val = '\b';
                return 2;

            case 'x': case 'X':
                s += 2;
                if (!(map[s[0]] & (HEX | DIGIT))) {


                    *val = 0;
                    error(&loc, "a hexadecimal escape sequence requires atleast one hex digit (0-F)");
                    return (s - olds);

                }
                while (map[s[0]] & (HEX | DIGIT)) {
                    (*val) <<= 4;
                     if (map[s[0]] & DIGIT) {
                        *val += (s[0] - 48);
                    } else if (map[s[0]] <= 70) {
                        *val += (s[0] - 55);
                    } else {
                        *val += (s[0] - 87);
                    }
                    s++;
                }

                if ((*val) > 0xFF) {


                    *val = 0xFF;
                    error(&loc, "A hexadecimal escape sequence cannot have a value greater than 0xFF");
                }
                return (s - olds);

            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                s++;
                for (int i = 0; (i < 3) && (map[s[0]] & DIGIT); i++, s++) {
                    if (s[0] == '8' || s[0] == '9')
                        break;
                    *val << 3;
                    *val += (s[0] - '0');
                }
                return (s - olds);
            default:
                return 0;
        }
    }
    return 0;
}

Token is_string() {
    ensure_buflen((512 + 3));
    char *s = buf.curr;
    char strbuf[512 + 1] = "";
    int escapechar = 0;
    int sbindex = 0;

    if (*s++ != '\"') {
        return ((void*)0);
    }
    while (*s != '\"') {
        if (sbindex >= 512) {
            error(&loc, "string too long. Max supported string length is 512 characters");

            while(*s != '\"') {
                s++;
                ensure_buflen((512 + 3));
            }
            s++;

            token tok = {.type=ERROR, .subtype=STR, .len = (s - buf.curr), .loc=loc};
            tok.val.strval = make_string(buf.curr, tok.len);
            return make_token(tok);
        }
        int esc_len = 0;
        if (*s == '\\' && (esc_len = is_escapeseq(&escapechar, s))) {
            strbuf[sbindex++] = (char)escapechar;
            s += esc_len;
        } else {
            strbuf[sbindex++] = *s;
            s++;
        }
    }
    s++;
    strbuf[sbindex] = '\0';
    token tok = {.type=STR, .len=(s - buf.curr), .loc=loc};
    tok.val.strval = make_string(strbuf, sbindex);
    return make_token(tok);
}


Token is_keyword() {
    ensure_buflen(32);
    char *s = buf.curr;
    int buflen = buf.bufend - buf.curr;
    token tok;
    switch (*s) {
        case 'a':
            if (buflen >= 5) {
                if (!strncmp(s, "auto", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=AUTO, .len=4, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 4);
                    return make_token(tok);
                }
            }
            break;
        case 'b':
            if (buflen >= 6) {
                if (!strncmp(s, "break", 5) &&
                    !(map[s[5]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=BREAK, .len=5, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 5);
                    return make_token(tok);
                }
            }
            break;
        case 'c':
            if (buflen >= 9) {
                if (!strncmp(s, "continue", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=CONTINUE, .len=8, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 8);
                    return make_token(tok);
                }
            }
            if (buflen >= 6) {
                if (!strncmp(s, "const", 5) &&
                    !(map[s[5]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=CONST, .len=5, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 5);
                    return make_token(tok);
                }
            }

            if (buflen >= 5) {
                if (!strncmp(s, "case", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=CASE, .len=4, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 4);
                    return make_token(tok);
                }
                if (!strncmp(s, "char", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=CHAR, .len=4, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 4);
                    return make_token(tok);
                }
            }
            break;
        case 'd':
            if (buflen >= 3) {
                if (!strncmp(s, "do", 2) &&
                    !(map[s[2]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=DO, .len=2, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }
            if (buflen >= 7) {
                if (!strncmp(s, "double", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=DOUBLE, .len=6, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 6);
                    return make_token(tok);
                }
            }
            if (buflen >= 8) {
                if (!strncmp(s, "default", 7) &&
                    !(map[s[7]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=DEFAULT, .len=7, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 7);
                    return make_token(tok);
                }
            }
            break;
        case 'e':
            if (buflen >= 5) {
                if (!strncmp(s, "else", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=ELSE, .len=4, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 4);
                    return make_token(tok);
                }
                if (!strncmp(s, "enum", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=ENUM, .len=4, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 4);
                    return make_token(tok);
                }
            }
            if (buflen >= 7) {
                if (!strncmp(s, "extern", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=EXTERN, .len=6, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 6);
                    return make_token(tok);
                }
            }
            break;
        case 'f':
            if (buflen >= 4) {
                if (!strncmp(s, "for", 3) &&
                    !(map[s[3]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=FOR, .len=3, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 3);
                    return make_token(tok);
                }
            }

            if (buflen >= 6) {
                if (!strncmp(s, "float", 5) &&
                    !(map[s[5]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=FLOAT, .len=5, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 5);
                    return make_token(tok);
                }
            }
            break;
        case 'g':
            if(buflen >= 5) {
                if (!strncmp(s, "goto", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=GOTO, .len=4, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 4);
                    return make_token(tok);
                }
            }
            break;
        case 'i':
            if(buflen >= 3) {
                if (!strncmp(s, "if", 2) &&
                    !(map[s[2]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=IF, .len=2, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }
            if (buflen >= 4) {
                if (!strncmp(s, "int", 3) &&
                    !(map[s[3]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=INT, .len=3, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 3);
                    return make_token(tok);
                }
            }
            if(buflen >= 7) {
                if (!strncmp(s, "inline", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=INLINE, .len=6, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 6);
                    return make_token(tok);
                }
            }
            break;
        case 'l':
            if(buflen >= 5) {
                if (!strncmp(s, "long", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=LONG, .len=4, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 4);
                    return make_token(tok);
                }
            }
            break;
        case 'r':
            if(buflen >= 7) {
                if (!strncmp(s, "return", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=RETURN, .len=6, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 6);
                    return make_token(tok);
                }
            }
            if (buflen >= 9) {
                if (!strncmp(s, "register", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=REGISTER, .len=8, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 8);
                    return make_token(tok);
                }
                if (!strncmp(s, "restrict", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=RESTRICT, .len=8, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 8);
                    return make_token(tok);
                }
            }
            break;
        case 's':
            if (buflen >= 6) {
                if (!strncmp(s, "short", 5) &&
                    !(map[s[5]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=SHORT, .len=5, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 5);
                    return make_token(tok);
                }
            }
            if(buflen >= 7) {
                if (!strncmp(s, "signed", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=SIGNED, .len=6, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 6);
                    return make_token(tok);
                }
                if (!strncmp(s, "sizeof", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=SIZEOF, .len=6, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 6);
                    return make_token(tok);
                }
                if (!strncmp(s, "static", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=STATIC, .len=6, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 6);
                    return make_token(tok);
                }
                if (!strncmp(s, "struct", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=STRUCT, .len=6, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 6);
                    return make_token(tok);
                }
                if (!strncmp(s, "switch", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=SWITCH, .len=6, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 6);
                    return make_token(tok);
                }
            }
            break;
        case 't':
            if(buflen >= 8) {
                if (!strncmp(s, "typedef", 7) &&
                    !(map[s[7]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=TYPEDEF, .len=7, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 7);
                    return make_token(tok);
                }
            }
            break;
        case 'u':
            if(buflen >= 6) {
                if (!strncmp(s, "union", 5) &&
                    !(map[s[5]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=UNION, .len=5, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 5);
                    return make_token(tok);
                }
            }
            if (buflen >= 9) {
                if (!strncmp(s, "unsigned", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=UNSIGNED, .len=8, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 8);
                    return make_token(tok);
                }
            }
            break;
        case 'v':
            if(buflen >= 5) {
                if (!strncmp(s, "void", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=VOID, .len=4, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 4);
                    return make_token(tok);
                }
            }
            if (buflen >= 9) {
                if (!strncmp(s, "volatile", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=VOLATILE, .len=8, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 8);
                    return make_token(tok);
                }
            }
            break;
        case 'w':
            if(buflen >= 6) {
                if (!strncmp(s, "while", 5) &&
                    !(map[s[5]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=WHILE, .len=5, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 5);
                    return make_token(tok);
                }
            }
            break;
        case '_':
            if(buflen >= 6) {
                if (!strncmp(s, "_Bool", 5) &&
                    !(map[s[5]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=_BOOL, .len=5, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 5);
                    return make_token(tok);
                }

            }
            if (buflen >= 8) {
                if (!strncmp(s, "_Atomic", 7) &&
                    !(map[s[7]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=_ATOMIC, .len=7, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 7);
                    return make_token(tok);
                }

            }
            if (buflen >= 9) {
                if (!strncmp(s, "_Alignas", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=_ALIGNAS, .len=8, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 8);
                    return make_token(tok);
                }
                if (!strncmp(s, "_Alignof", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=_ALIGNOF, .len=8, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 8);
                    return make_token(tok);
                }
                if (!strncmp(s, "_Complex", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=_COMPLEX, .len=8, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 8);
                    return make_token(tok);
                }
                if (!strncmp(s, "_Generic", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=_GENERIC, .len=8, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 8);
                    return make_token(tok);
                }

            }
            if (buflen >= 10) {
                if (!strncmp(s, "_Noreturn", 9) &&
                    !(map[s[9]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=_NORETURN, .len=9, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 9);
                    return make_token(tok);
                }

            }
            if (buflen >= 11) {
                if (!strncmp(s, "_Imaginary", 10) &&
                    !(map[s[10]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=_IMAGINARY, .len=10, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 10);
                    return make_token(tok);
                }

            }
            if (buflen >= 14) {
                if (!strncmp(s, "_Thread_local", 13) &&
                    !(map[s[13]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=_THREAD_LOCAL, .len=13, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 13);
                    return make_token(tok);
                }
            }
            if (buflen >= 15) {
                if (!strncmp(s, "_Static_assert", 14) &&
                    !(map[s[14]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=_STATIC_ASSERT, .len=14, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 14);
                    return make_token(tok);
                }
            }
            break;
        default:
            return ((void*)0);
    }
    return ((void*)0);
}






Token is_punct() {
    ensure_buflen(3);
    char *s = buf.curr;
    int buflen = buf.bufend - buf.curr;
    switch(*s) {
        case '[': case ']': case '(': case ')': case '{':
        case '}': case '~': case ',': case '?': case ';':
        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '+':
            if (buflen >= 2) {

                if (s[1] == '+') {
                    token tok = {.type=PUNCT, .subtype=INCR, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);

                } else if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=INCREQ, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }

        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '-':
            if (buflen >= 2) {

                if (s[1] == '-') {
                    token tok = {.type=PUNCT, .subtype=DECR, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);

                } else if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=DECREQ, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);

                } else if (s[1] == '>') {
                    token tok = {.type=PUNCT, .subtype=ARROW, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }

        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '<':
            if (buflen >= 3) {

                if (s[1] == '<' && s[2] == '=') {
                    token tok = {.type=PUNCT, .subtype=LSHFTEQ, .len=3, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 3);
                    return make_token(tok);
                }
            }
            if (buflen >= 2) {

                if (s[1] == '<') {
                    token tok = {.type=PUNCT, .subtype=LSHFT, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);

                } else if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=LEQ, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);

                } else if (s[1] == ':') {
                    token tok = {.type=PUNCT, .subtype=LSQBRACREP, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);

                } else if (s[1] == '%') {
                    token tok = {.type=PUNCT, .subtype=LCBRACREP, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }

        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '>':
            if (buflen >= 3) {

                if (s[1] == '>' && s[2] == '=') {
                    token tok = {.type=PUNCT, .subtype=RSHFTEQ, .len=3, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 3);
                    return make_token(tok);
                }
            }
            if (buflen >= 2) {

                if (s[1] == '>') {
                    token tok = {.type=PUNCT, .subtype=RSHFT, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);

                } else if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=GEQ, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }

        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '=':
            if (buflen >= 2) {

                if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=EQ, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }

        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '!':
            if (buflen >= 2) {
                if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=NEQ, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }

        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '&':
            if (buflen >= 2) {

                if (s[1] == '&') {
                    token tok = {.type=PUNCT, .subtype=AND, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);

                } else if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=ANDEQ, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }
        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '|':
            if (buflen >= 2) {

                if (s[1] == '|') {
                    token tok = {.type=PUNCT, .subtype=OR, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);

                } else if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=OREQ, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }

        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '*':
            if (buflen >= 2) {

                if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=MULTEQ, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }

        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '/':
            if (buflen >= 2) {

                if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=DIVEQ, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }

        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '%':
            if (buflen >= 4) {

                if (s[1] == ':' && s[2] == '%' && s[3] == ':') {
                    token tok = {.type=PUNCT, .subtype=HHASHREP, .len=4, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 4);
                    return make_token(tok);
                }
            }
            if (buflen >= 2) {

                if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=MODEQ, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);

                } else if (s[1] == '>') {
                    token tok = {.type=PUNCT, .subtype=RCBRACREP, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);

                } else if (s[1] == ':') {
                    token tok = {.type=PUNCT, .subtype=HASHREP, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }

        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '#':
            if (buflen >= 2) {

                if (s[1] == '#') {
                    token tok = {.type=PUNCT, .subtype=HHASH, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }

        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case ':':
            if (buflen >= 2) {

                if (s[1] == '>') {
                    token tok = {.type=PUNCT, .subtype=RSQBRACREP, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 1);
                    return make_token(tok);
                }
            }

        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '.':
            if (buflen >= 3) {

                if (s[1] == '.' && s[2] == '.') {
                    token tok = {.type=PUNCT, .subtype=ELLIPS, .len=3, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 3);
                    return make_token(tok);
                }
            }

        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '^':
            if (buflen >= 2) {

                if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=XOREQ, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }

        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        default:
            return ((void*)0);
    }
    return ((void*)0);
}

void process_gnu_directive() {
    ensure_buflen((512 + 3));

    char *s = buf.curr;

    if (*buf.curr == '#') {
        buf.curr += 2;
    }

    Token tok = ((void*)0);
    tok = is_intconst();
    if ((tok != ((void*)0)) && (tok->type == INTCONST)) {
        buf.curr++;
        loc.y = tok->val.intval.i;
        loc.x = 1;

        loc.nf = 0; loc.rtf = 0; loc.shf = 0; loc.tae = 0;
    } else {
        error(&loc, "internal error while processing gnu cpp directive");
    }

    tok = ((void*)0);
    tok = is_string();
    if ((tok != ((void*)0)) && (tok->type == STR)) {
        loc.file = tok->val.strval;
    } else {
        error(&loc, "internal error while processing gnu cpp directive");
    }


    while (1) {
        if (*buf.curr == '\n') {
            loc.x = 1;
            buf.curr++;
            return;
        }
        switch (buf.curr[1]) {
            case '1':
                loc.nf = 1;
                buf.curr += 2;
                break;
            case '2':
                loc.rtf = 1;
                buf.curr += 2;
                break;
            case '3':
                loc.shf = 1;
                buf.curr += 2;
                break;
            case '4':
                loc.tae = 1;
                buf.curr += 2;
                break;
            default:
                error(&loc, "internal error while processing gnu cpp directive");
        }
    }



}





Token make_token(token tok) {
    Token result = alloc(sizeof(tok), PERM);
    *result = tok;

    buf.curr += tok.len;
    loc.x += tok.len;

    return result;
}
