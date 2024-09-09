# 1 "tests/parse-tests/plutoheader.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/stdc-predef.h" 1
# 32 "<command-line>" 2
# 1 "tests/parse-tests/plutoheader.c"




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
# 6 "tests/parse-tests/plutoheader.c" 2
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
# 7 "tests/parse-tests/plutoheader.c" 2
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
# 8 "tests/parse-tests/plutoheader.c" 2
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/assert.h" 1
# 19 "/afs/.ir/users/j/k/jk10/lib/musl/include/assert.h"
_Noreturn void __assert_fail (const char *, const char *, int, const char *);
# 9 "tests/parse-tests/plutoheader.c" 2
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/stddef.h" 1
# 19 "/afs/.ir/users/j/k/jk10/lib/musl/include/stddef.h"
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h" 1
# 41 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h"
typedef struct { long long __ll; long double __ld; } max_align_t;
# 60 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h"
typedef long ptrdiff_t;
# 20 "/afs/.ir/users/j/k/jk10/lib/musl/include/stddef.h" 2
# 10 "tests/parse-tests/plutoheader.c" 2
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
# 11 "tests/parse-tests/plutoheader.c" 2
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/libgen.h" 1







char *dirname(char *);
char *basename(char *);
# 12 "tests/parse-tests/plutoheader.c" 2
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
# 13 "tests/parse-tests/plutoheader.c" 2
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/limits.h" 1





# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/alltypes.h" 1
# 7 "/afs/.ir/users/j/k/jk10/lib/musl/include/limits.h" 2
# 14 "tests/parse-tests/plutoheader.c" 2
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
# 15 "tests/parse-tests/plutoheader.c" 2
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
# 16 "tests/parse-tests/plutoheader.c" 2
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/errno.h" 1
# 10 "/afs/.ir/users/j/k/jk10/lib/musl/include/errno.h"
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/errno.h" 1
# 11 "/afs/.ir/users/j/k/jk10/lib/musl/include/errno.h" 2




int *__errno_location(void);
# 17 "tests/parse-tests/plutoheader.c" 2
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/float.h" 1







int __flt_rounds(void);
# 46 "/afs/.ir/users/j/k/jk10/lib/musl/include/float.h"
# 1 "/afs/.ir/users/j/k/jk10/lib/musl/include/bits/float.h" 1
# 47 "/afs/.ir/users/j/k/jk10/lib/musl/include/float.h" 2
# 18 "tests/parse-tests/plutoheader.c" 2
# 37 "tests/parse-tests/plutoheader.c"
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
typedef unsigned int size_t;


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
    char buf[4096];
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


# 1 "tests/parse-tests/../../include/token.h" 1
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
# 137 "tests/parse-tests/plutoheader.c" 2


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

# 1 "tests/parse-tests/../../include/node.h" 1
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
# 239 "tests/parse-tests/plutoheader.c" 2

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
static char temp_dir[4096];
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

struct s {
    int i;
    int ii;
    int arr[];
} ss;
Type comp(Type composite_type);
int func(Node n);

enum ee {
    BLUE = 1, RED = 3
} eenum;

void addscalar(int n, int m, double a[n][n*m+300], double x);

int main() {
    double b[4][308];
    addscalar(4, 2, b, 2.17);
    return 0;
}
void addscalar(int n, int m, double a[n][n*m+300], double x) {
    for (int i = 0; i < n; i++)
        for (int j = 0, k = n*m+300; j < k; j++)

            a[i][j] += x;
}
