/*

    Symbol table interface
    struct table;
    typedef stuct table *Table;
    
    namespaces - (1) statement labels, (2) tags, (3) struct/union members, (4) ordinary identifiers
    we can artificially create a few more for unscoped collections

    functionality - 
    1. lookup a name given a level - return a symbol (either new or old)
    2. add a new level
    3. create a new label and increment gloab counter
    4. find a label - return a symbol 
    5. generate a variable
    citation: a large portion of this page has been sourced from DRH & CWF's book, a retargetable
              c compiler. this implies that this particular file shouldn't be considered as original
              work.
*/
#include "pluto.h"

unsigned int level = GLOBAL;

// intialise symbol tables
static table cns = {.level = CONSTANTS, .prev = NULL, .all = NULL};
// initialize identifiers, globals, externals with global level
static table ext = {.level = GLOBAL, .prev = NULL, .all = NULL};
static table idn = {.level = GLOBAL, .prev = NULL, .all = NULL};
static table typ = {.level = GLOBAL, .prev = NULL, .all = NULL};

Table constants = &cns;
Table external = &ext;
Table globals = &idn;
Table identifiers = &idn;
Table types = &typ;
// table is created an destroyed for each function
Table labels;

/*
    appends a new table with level = lev
*/
Table make_table(Table tb, unsigned int lev) {
    Table nw = alloc(sizeof(table), FUNC);
    nw->level = lev;
    nw->prev = tb;
    nw->all = NULL;
    if (tb)
        nw->all = tb->all;
    return nw;
}



unsigned int enterscope() {
    return ++level;
}

unsigned int exitscope() {
    assert(level >= MIN_LEVEL);
    rmtypes(level);
    if (level >= MIN_LEVEL) {
        if (types->level == level)
            types = types->prev;
        if (identifiers->level == level)
            identifiers = identifiers->prev;
        if (level == PARAM) 
            labels = NULL;
    }
    // don't do anything to the table if level was greater than the tables level
    return --level;
}

/*
    installs a new symbol entry into tb. appends a new table
    if tb did not exits or if lev is greater that tb's level
*/
Symbol install(char *name, Table *tb, unsigned int lev, int an) {
    assert(lev >= MIN_LEVEL);
    // first try to locate the table
    Table tbcpy = *tb;
    
    // if current level is lower than level append new table with lev
    if (!*tb || tbcpy->level < lev) 
        tbcpy = *tb = make_table(tbcpy, lev);
    // at this point if tbcpy isn't of lev (since it shouldn't be greater 
    // then it's an internal error
    assert(tbcpy->level == lev);
    
    // bindly install name into tbcpy
    struct entry *e = alloc(sizeof(struct entry), an);
    e->sym.name = name;
    e->sym.scope = lev;
    //e->sym.loc = loc;
    e->sym.prev = tbcpy->all;
    // type and sclass to be filled externally by client
    // add to hash table (prepend in hash bucket)
    int h = hash(name, strlen(name), TAB_HASH_SIZE);
    e->next = tbcpy->buckets[h];
    tbcpy->buckets[h] = e;
    return &(e->sym);
}

/*  
    looks for a symbol who's name field is name in the current and all enclosing scopes
*/
    
Symbol lookup(char *name, Table tb) {
    struct entry *e;
    int h = hash(name,strlen(name), TAB_HASH_SIZE);
    while (tb) {
        // go through the hash chain to look for name 
        e = tb->buckets[h]; 
        while (e) {
            // since all strings are interned we only need to compare the address
            if (e->sym.name == name) 
                return &(e->sym);
            e = e->next;
        }
    tb = tb->prev;
    }
    // not found
    return NULL;
}

int genlabel(int n) {
    static int label = 1;

    label += n;
    return label - n;
}

/* TODO: marked as comment since incomplete 
Symbol make_constant(type ty, constantval c) {
    struct entry *e; 
    unsigned int h = c.ui & (TAB_HASH_SIZE - 1);
    ty = unqual(ty); 
    for (e = constants->buckets[h]; e; e = e->next) {
        if (eqtype(ty, p->sym.type, 1)) {
            // return symbol entry if values are equal
            switch (ty->id) {
                case UCHAR:     if (c.uc == p->sym.u.c.v.uc)    return &p->sym; break;
                case SCHAR:     if (c.c == p->sym.u.c.v.c)      return &p->sym; break;
                case SHORT:     if (c.sh == p->sym.u.c.v.sh)    return &p->sym; break;
                case ULLONG:    if (c.ull == p->sym.u.c.v.ull)  return &p->sym; break;
                case SLLONG:    if (c.ll == p->sym.u.c.v.ll)    return &p->sym; break;
                case ULONG:     if (c.ul == p->sym.u.c.v.ul)    return &p->sym; break;
                case SLONG:     if (c.l == p->sym.u.c.v.l)      return &p->sym; break;
                case UINT:      if (c.ui == p->sym.u.c.v.ui)    return &p->sym; break;
                case SINT:      if (c.i == p->sym.u.c.v.i)      return &p->sym; break;
                case SDOUBLE:   if (c.d == p->sym.u.c.v.d)      return &p->sym; break;
                case LDOUBLE:   if (c.ld == p->sym.u.c.v.ld)    return &p->sym; break;
                case SFLOAT:    if (c.f == p->sym.u.c.v.f)      return &p->sym; break;
                case ARRAY: case FUNCTION:
                case POINTER:   if (c.p == p->sym.u.c.v.p)      return &p->sym; break;
            }

        }
    }
    e = alloc(sizeof(struct entry), PERM); 
    e->sym.name = vtoa(ty, c); 
    e->sym.scope = CONSTANTS; 
    e->sym.type = ty;
    e->sym.sclass = STATIC; 
    e->sym.u.c.v = c; 
    e->link = constants->buckets[h]; 
    e->sym.up = constants->all; 
    constants->all = &e->sym; 
    constants->buckets[h] = e;
    e->sym.defined = 1;
    return &e->sym;
}

*/
// TODO: labels, intconst, generated variables, temporary variables
