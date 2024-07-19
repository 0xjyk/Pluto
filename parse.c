#include "pluto.h"
buffer buf; 
location loc;

void parse(char *pp_file) {
    // initionalise the buffer
    bufinit(pp_file);
    int tok;
    while (tok = lex()) {
        // do parse stuff
    }
}

void print_cpp() {
    while(buf.curr < buf.bufend) {
        while (buf.curr < buf.bufend) {
            fprintf(stdout, "%c", *buf.curr++);
        }
        fillbuf();
    }
}
