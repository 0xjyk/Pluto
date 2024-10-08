/*
    file: pluto.c
    description: pluto.c contains the main driver for the C11 compiler.
    implementation notes: 
        1. extract CLI options and files from stdin
            - while extracting options update global variables
        2. preprocess all files (a better route could have been to pp files one at a time)
        For each file - 
        3. lex & parse 
            - creates a file consisting of the parse IR
        4. typecheck the parse IR 
        5. generate ASM for IR
            
    author: jasraj kripalani
*/
#include "pluto.h"

int main(int argc, char *argv[]) {
    if (atexit(remove_tempfiles)) {
        fprintf(stderr, "Couldn't set exit function\n");
        exit(EXIT_FAILURE);
    }

    // extract CLI options and files from stdin
    parseoptions(argc, argv);
    
    // preprocess all files
    preprocess();

    // lex & parse each file
    for (int i = 0; i < ppfiles->size; i++) {
        // parse seeks tokens from lex, which in-turn
        // reads a stream of characters from ppfiles, converting them into tokens
        if (JOBSCOPE == LEX) {
            // call lexdriver to perform only lexing 
            lexdriver((char *)vec_get(ppfiles, i));  
        } else if (JOBSCOPE == PARSE) {
            Node root = parse((char *)vec_get(ppfiles, i));
            //dump_stringpool();
            create_sudumpfile((char *)vec_get(ppfiles, i));
            if (!typesfile) {
            }
            dump_AST(root, 0);
        } else if (JOBSCOPE == TYPECHECK) {
            dump_cpp((char *)vec_get(ppfiles, i));
        } else {
            printf("requested l, p, t, c\n"); 
        }
        if (num_errors) 
            printf("\n%d error(s) encountered\n", num_errors);
    }
    
    exit(EXIT_SUCCESS);
}

