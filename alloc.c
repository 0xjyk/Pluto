#include "pluto.h"
/*
    file: alloc.c
    description: implements a substitute for malloc and free. specifically, 
    implements allocation bases the allocation on the object lifetime since 
    most allocations and deallocations occur in well defined chucks. This 
    approach was suggested by DRH and CWF in their book "a retargetable c compiler"
    implementation notes: 
        - objects with the same lifetimes are allocated from the same arena
        - deallocation (or invalidation) of the entire arena is done at once
        - each arena is a list of pages
        - yes, this method is pretty memory greedy, but this doesn't 
          turn out too bad in practice because individual allocations are small
          and frees are done all at once. This strategy would be terrible for large requests
        
    author: jasraj kripalani

*/


#define PG_SIZE 4096
#define ARENA_MAX 3
// works only for multiples of two
#define ROUNDUP2(a, mult) (((a) + ((mult)- 1)) & ~((mult) - 1))


// An array of first blocks for three arenas
static block first[] = { {NULL}, {NULL}, {NULL} };
static Block arena[] = { &first[0], &first[1], &first[2] };

// freelist holds previously malloced, but now free blocks
static Block freelist = NULL;




void *alloc(unsigned long sz, int an) {
    assert(sz >= 0 && an >= 0 && an < ARENA_MAX);
    // normalise request size 
    sz = ROUNDUP2(sz, sizeof(union align));
    Block ap = arena[an];

    // block doesn't have sufficient space, first allocate 
    if (ap->avail + sz > ap->limit) {
        // make a request in page size multiples 
        unsigned long rsz = ROUNDUP2(sz, PG_SIZE);
        
        // first try to retrieve the page from the freelist
        Block fl = freelist;
        Block prev = fl;
        while(fl) {
            // reset start of block
            fl->avail = (char*)fl + sizeof(block);
            // found an appropriate block
            if (fl->avail + rsz <= fl->limit) {
                // remove from freelist     
                if (prev == freelist) {
                    freelist = fl->next;
                } else {
                    prev->next = fl->next;
                }
                // add to arena[an]
                ap->next = fl;
                ap = ap->next;
                ap->next = NULL;
                ap->avail = (char *)ap + sizeof(block) + sz;
                return ap->avail - sz;
            }
            prev = fl; 
            fl = fl->next;
        }
        // couldn't find a block in the freelist, use malloc 
        ap->next = malloc(rsz); 
        if (ap->next == NULL) 
            fprintf(stderr, "insufficient memory\n");
        ap = ap->next; 
        ap->next = NULL; 
        ap->avail = (char *)ap + sizeof(block) + sz; 
        ap->limit = (char *)ap + rsz;
        return ap->avail - sz; 
    }
    ap->avail += sz; 
    return ap->avail - sz; 
}

void dealloc(int an) {
    // add blocks to freelist and update freelist
    assert(an >= 0 && an < ARENA_MAX); 
    arena[an]->next = freelist; 
    freelist = first[an].next;
    first[an].next = NULL;
    arena[an] = &first[an];
}

// returns an arena allocated, null-terminated copy consisting ofsz bytes of str
char *strloc(char *str, int sz, int an) {
    assert(sz >= 0 && an >= 0 && an < ARENA_MAX); 
    // allocate space in arena an
    char *s = (char *)alloc(sz + 1, an);
    // copy str into the newly allocated space
    char *ss = s; 
    for (int i = 0; i < sz; i++, ss++, str++) 
        *ss = *str;
    *ss = '\0';
    return s;
}

void *new(void *ptr, int an) {
    assert(ptr != NULL && an >= 0 && an < ARENA_MAX); 
}

