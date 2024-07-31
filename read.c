#include "pluto.h"
/*
    file: read.c 
    description: read.c provides functionality to read 
    the contents of a file, one line at a time
    implementation notes: 
        - reads BUFLEN bytes into the buffer
        - buffer is used to used to reduce I/O calls
        - buffer gets refilled via calls to fillbuf
        - it is possible for the filbuf to be called 
          "prematurely", i.e. when the entire buffer
          hasn't been read fully, in this case the 
          remaining contents are placed in the begining
          and then BUFLEN bytes are appended. 
        - for every ppfile 
            - initialise the buffer
            - lex starts consuming characters and emiting tokens
            - when characters go below a threshold, lex calls fillbuf
            - when lex has completed lexing the file it returns a senital token
            - so that the driver can assign it the next file

    author: jasraj kripalani
*/

int pp_read_complete;

void fillbuf() {
    // first check to see if some characters weren't read - place them at the start
    char *tempcurr = buf.buf;
    if (buf.curr < buf.bufend) {
        while (buf.curr <= buf.bufend) {
            *tempcurr++ = *buf.curr++;
        }
    }
    assert((&buf.buf[BUFSIZE-1] - tempcurr) >= BUFLEN);
    // read BUFLEN bytes into buffer starting at tempcurr
    ssize_t bytes_read = read(buf.fd, tempcurr, BUFLEN); 
    if (bytes_read < 0) {
        printf("couldn't read %s\n", buf.file);
        exit(EXIT_FAILURE);
    }
    pp_read_complete = !(bytes_read);
    buf.bufend = tempcurr + bytes_read;
    // in either case buf->curr starts at the begining of the buffer
    buf.curr = buf.buf;
}

// initialise the buffer 
void bufinit(char *pp_file) {
    // attempt to open file
    int fd = open(pp_file, O_RDONLY);
    if (fd <= -1) {
        printf("couldn't open %s\n", pp_file);
        exit(EXIT_FAILURE);
    }
    // setup buf
    buf.file = pp_file; 
    buf.fd = fd;
    buf.curr = buf.buf; 
    // will be updated accurately by fillbuf
    buf.bufend = buf.buf;
    // fill buf
    fillbuf();
    loc.file = pp_file;
    loc.x = 0;
    loc.y = 1;
}

void ensure_buflen(int len) {
    if (((buf.bufend - buf.curr) < len) && !pp_read_complete) {
        fillbuf();
    }
}

