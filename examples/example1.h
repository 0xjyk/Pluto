#ifndef INIT1_H
#define INIT1_H
#define NULL ((void *)0)
// some structure definitions
typedef struct red{
    int i; 
} red;
typedef struct green{
    int i; 
} green; 

typedef struct blue{
    int i; 
} blue;

typedef struct teal{
    int i; 
} teal; 

typedef struct lavender{
    int i; 
} lavender;

// introduce new aliases 
typedef red *Red;
typedef green *Green;
typedef blue *Blue;
typedef teal *Teal;
typedef lavender *Lavender;

// three compatible structures 
struct s1 {
    _Bool flag:1;
    struct red *r;
    struct green *g;
    struct blue *b;
    struct teal *t;
    struct lavender *l;
};

struct s2 {
    _Bool flag:1;
    red *r; 
    green *g;
    blue *b;
    teal *t;
    lavender *l;
};
typedef void (*Callback)(void*);

struct ComplexStruct {
    union {
        struct { int x, y; } point;
        struct { float radius; } circle;
    } geometry;
    struct {
        unsigned is_visible : 1;
        unsigned is_active : 1;
        unsigned char type : 6;
    } flags;
    Callback on_update;
    struct ComplexStruct* next;
};

#endif
