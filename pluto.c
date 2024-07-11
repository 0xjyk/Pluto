/*
    file: pluto.c
    description: pluto.c contains the main driver for the C11 compiler.
    implementation notes: 
        1. extract CLI options and files from stdin
            - while extracting options update global variables
            
    author: Jasraj Kripalani
*/
#include "pluto.h"



int main(int argc, char *argv[]) {
    if (!atexit(remove_tempfiles)) {
        fprintf(stderr, "Couldn't set exit function\n");
        exit(EXIT_FAILURE);
    }

    // extract CLI options and files from stdin
    parseoptions(argc, argv);


    // preprocess all files
    preprocess();
    
    // lex & parse each file


    

}















