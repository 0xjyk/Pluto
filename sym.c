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



*/
#include "pluto.h"

unsigned int level = GLOBAL;

// intialise symbol tables
static table cns = {.level = CONSTANTS, .prev = NULL, .all = NULL};
// initialize identifiers, globals, externals with global level
static table ext = {.level = GLOBAL, .prev = NULL, .all = NULL};
static table glb = {.level = GLOBAL, .prev = NULL, .all = NULL};
static table idn = {.level = GLOBAL, .prev = NULL, .all = NULL};
static table typ = {.level = GLOBAL, .prev = NULL, .all = NULL};

Table constants = &cns;
Table external = &ext;
Table globals = &glb;
Table identifiers = &idn;
Table types = &typ;
Table lables;

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
    assert(level > MIN_LEVEL);
    if (level >= MIN_LEVEL) {
        // TODO: remove types 

        // TODO: remove identifiers

    }
    // don't do anything if level is already at its lowest
    return --level;
}

/*
    installs a new symbol entry into tb. appends a new table
    if tb did not exits or if lev is greater that tb's level
*/
Symbol install(char *name, Table tb, unsigned int lev, int an) {
    assert(lev > 0);
    // first try to locate the table
    Table tbcpy = tb;
    
    // if tb didn't exit to being with create the table with lev
    if (!tb)
        tbcpy = make_table(NULL, lev);
    // if current level is lower than level append new table with lev
    if (tb->level < lev) 
        tbcpy = make_table(tb, lev);
    // at this point if tbcpy isn't of lev then it's an internal error
    assert(tbcpy->level == lev);
    
    // bindly install name into tbcpy
    struct entry *e = alloc(sizeof(struct entry), an);
    e->sym.name = name;
    e->sym.scope = lev;
    e->sym.loc = loc;
    e->sym.prev = tbcpy->all;
    // type and sclass to be filled externally by client
    // add to hash table (prepend in hash bucket)
    int h = hash(name, TAB_HASH_SIZE);
    e->next = tbcpy->buckets[h];
    tbcpy->buckets[h] = e;
    return &(e->sym);
}

/*  
    looks for a symbol who's name field is name in the current and all enclosing scopes
*/
    
Symbol lookup(char *name, Table tb) {
    struct entry *e;
    int h = hash(name, TAB_HASH_SIZE);
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
    static int label= 1;

    label += n;
    return label - n;
}



