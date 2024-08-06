#include "pluto.h"

#define NUM_BUCKETS 1024
#define INT_MAX_LEN (10 + 1 + 1)
static struct string {
    char *str; 
    int len; 
    struct string *next; 
} *buckets[NUM_BUCKETS];


// djb2, a simple hash function by dan bernstein
int hash(unsigned char *str, int num_buckets) {
    unsigned long hash = 5381;
    int c;
    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    // ensure hash is between 0-NUM_BUCKETS
    hash &= (num_buckets - 1);
    return hash;
};

char *make_string(char *str, int len) {
    // hash str to get the appropriate bucket
    struct string **strlist = &buckets[hash(str, NUM_BUCKETS)];
    struct string *s = *strlist;

    // if string already in string pool, return existing string
    while(s) {
        // If both lens don't match, no need to check contents
        if (s->len == len) {
            char *so = str; 
            char *st = s->str; 
            int i = len;
            while (i && (*so++ == *st++)) {
                i--; 
            }
            // its a match if len chars were seen successfully and the end chars matched
            if (i == 0) {
                return s->str;
            }
        }
        s = s->next;
    }
    // this portion executes when the string is a new addition to the string pool
    struct string *ss = alloc(sizeof(struct string), PERM); 
    ss->str = strloc(str, len, PERM); 
    ss->len = len; 
    ss->next = *strlist; 
    *strlist = ss;
    return ss->str;
}

char *dtos (int n) {
    char buf[INT_MAX_LEN]; 
    char *s = &buf[INT_MAX_LEN - 1];
    *s = '\0';
    char is_neg = 0;
    if (n < 0) {
        is_neg = 1;
        n = -n;
    }
    do {
        int m = n % 10;
        *--s = '0' + m;
        n /= 10;
    } while (n != 0);
    if (is_neg) 
        *--s = '-';
    
    return make_string(s, INT_MAX_LEN - (s - buf));
}

// print only the buckets that contain strings
void dump_stringpool() {
    for(int i = 0; i < NUM_BUCKETS; i++ ) {
        struct string *s = buckets[i]; 
        if (s) {
            fprintf(stdout, "Bucket [%d]\n", i); 
            while (s) {
                fprintf(stdout, "> \"%s\"\n", s->str);
                s = s->next;
            }
        }
    }
}




