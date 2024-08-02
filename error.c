#include "pluto.h"

int num_warnings = 0;
int num_errors = 0;

void warning(Location l, const char *msg) {
    num_warnings++;
    fprintf(stderr, "%s:%d:%d warning: %s\n", l->file, l->y, l->x, msg);
}

void error(Location l, const char *msg) {
    //filename:line:offset: error : msg
    num_errors++;
    fprintf(stderr, "%s:%d:%d error: %s\n", l->file, l->y, l->x, msg);
}
