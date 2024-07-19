#include "pluto.h"
#include "include/charmap.h"

token t;


int lex() {
   // keep lexing till there's no more input in the buffer
    while (buf.curr < buf.bufend) {
        // while the buffer contains characters read them 
        while (buf.curr < buf.bufend) {
            // if buffer contains less then MINBUFLEN characters refill - done to simplify reading
            if ((buf.bufend - buf.curr) <= MINBUFLEN && !pp_read_complete) 
                fillbuf();
            // check if next token is a keyword
            int token_len = 0;
            if (map[*buf.curr] & LETTER) {
                // check if it's a keyword 
                if (token_len = is_keyword()) {
                    // update the global token with the token details
                    t.type = KEYWORD;
                    t.val = strloc(buf.curr, token_len, PERM);
                    t.loc = loc;
                    buf.curr += token_len; 
                    // update file location
                    if (map[*buf.curr] & NEWLINE) {
                        loc.x = 0; 
                        loc.y++; 
                    } else {
                        loc.x += token_len;
                    }
                    return KEYWORD;
                // check if it's an identifier
                } else if (token_len = is_identifier()) {
                    t.type = ID; 
                    t.val = strloc(buf.curr, token_len, PERM); 
                    t.loc = loc; 
                    buf.curr += token_len; 
                    // update file location
                    if (map[*buf.curr] & NEWLINE) {
                        loc.x = 0; 
                        loc.y++; 
                    } else {
                        loc.x += token_len;
                    }
                    return ID;
                }
                // error: TODO
                return -1;
            }
            // TODO: process GNU cpp directives
            if (*buf.curr == '#') {
                while (*buf.curr++ != '\n') {}
                loc.x = 0; 
            } else {
                buf.curr++; 
            }
        }
        fillbuf(); 
    }
    return EOI;
}

// only used with client requests for a lex file - mainly as a debugging aid for this project
void lexdriver(char *pp_file) {
    // initialise the buffer to facilitate reading
    bufinit(pp_file); 
    
    // create the output file
    char lex_file[PATH_MAX]; 
    *lex_file = '\0';
    strcat(lex_file, pp_file);
    lex_file[strlen(lex_file)-2] = 'l';
    lex_file[strlen(lex_file)-1] = 'l';
    FILE *fp = fopen(lex_file, "w");
    //mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; 
    //int fd = open(lex_file, O_WRONLY, O_CREAT, O_TRUNC, mode);
    if (fp == NULL) {
        printf("couldn't open %s\n", lex_file); 
        exit(EXIT_FAILURE);
    }

    int tok; 
    while ((tok = lex()) != EOI) {
        if (tok == ID) {
           fprintf(fp, "ID \"%s\"\n", t.val);
           fprintf(fp, "Location: %d, %d\n", t.loc.x, t.loc.y);
        } else if (tok == KEYWORD) {
            fprintf(fp, "KEYWORD \"%s\"\n", t.val);
            fprintf(fp, "Location: %d, %d\n", t.loc.x, t.loc.y);
        } else {
            fprintf(fp, "ERROR!\n");
        }
    }
}

int is_identifier() {
    char *s = buf.curr; 
    // keep reading as long 
    while (s <= buf.bufend && 
            map[*s] & (DIGIT|LETTER)) {
        s++;
    }
    // anything that's not a digit or letter terminates id
    int token_len = s - buf.curr;
    return token_len;
}

int is_keyword() {
    char *s = buf.curr; 
    int buflen = buf.bufend - buf.curr;
    // before checking for a keyword make sure that the buffer 
    // has sufficient characters (space for the keyword plus a terminating BLANK
    switch (*s) {
        case 'a': 
            if (buflen >= 5) {
                if (!strncmp(s, "auto", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    return 4;
                }
            } 
            break;
        case 'b':
            if (buflen >= 6) {
                if (!strncmp(s, "break", 5) &&
                    !(map[s[5]] & (DIGIT|LETTER))) {
                    return 5;
                }
            }
            break;
        case 'c':
            if (buflen >= 5) {
                if (!strncmp(s, "case", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    return 4;
                }
                if (!strncmp(s, "char", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    return 4;
                }
            }
            if (buflen >= 6) {
                if (!strncmp(s, "const", 5) &&
                    !(map[s[5]] & (DIGIT|LETTER))) {
                    return 5;
                }
            }
            break;
        case 'd': 
            if (buflen >= 3) {
                if (!strncmp(s, "do", 2) &&
                    !(map[s[2]] & (DIGIT|LETTER))) {
                    return 2;
                }
            }
            if (buflen >= 7) {
                if (!strncmp(s, "double", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    return 6;
                }
            }
            if (buflen >= 8) {
                if (!strncmp(s, "default", 7) &&
                    !(map[s[7]] & (DIGIT|LETTER))) {
                    return 7;
                }
            }
            break;
        case 'e': 
            if (buflen >= 5) {
                if (!strncmp(s, "else", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    return 4;
                }
                if (!strncmp(s, "enum", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    return 4;
                }
            }
            if (buflen >= 7) {
                if (!strncmp(s, "extern", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    return 6;
                }
            }
            break;
        case 'f':
            if (buflen >= 4) {
                if (!strncmp(s, "for", 3) &&
                    !(map[s[3]] & (DIGIT|LETTER))) {
                    return 3;
                }
            }

            if (buflen >= 6) {
                if (!strncmp(s, "float", 5) &&
                    !(map[s[5]] & (DIGIT|LETTER))) {
                    return 5;
                }
            }
            break;
        case 'g': 
            if(buflen >= 5) {
                if (!strncmp(s, "goto", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    return 4;
                }
            }
            break;
        case 'i':
            if(buflen >= 3) {
                if (!strncmp(s, "if", 2) &&
                    !(map[s[2]] & (DIGIT|LETTER))) {
                    return 2;
                }
            }
            if (buflen >= 4) {
                if (!strncmp(s, "int", 3) &&
                    !(map[s[3]] & (DIGIT|LETTER))) {
                    return 3;
                }
            }
            if(buflen >= 7) {
                if (!strncmp(s, "inline", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    return 6;
                }
            }
            break;
        case 'l':
            if(buflen >= 5) {
                if (!strncmp(s, "long", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    return 4;
                }
            }
            break;
        case 'r':
            if(buflen >= 7) {
                if (!strncmp(s, "return", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    return 6;
                }
            }
            if (buflen >= 9) {
                if (!strncmp(s, "register", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    return 8;
                }
                if (!strncmp(s, "restrict", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    return 8;
                }
            }
            break;
        case 's':
            if (buflen >= 6) {
                if (!strncmp(s, "short", 5) && 
                    !(map[s[5]] & (DIGIT|LETTER))) {
                    return 5; 
                }
            }
            if(buflen >= 7) {
                if (!strncmp(s, "signed", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    return 6;
                }
                if (!strncmp(s, "sizeof", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    return 6;
                }
                if (!strncmp(s, "static", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    return 6;
                }
                if (!strncmp(s, "struct", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    return 6;
                }
                if (!strncmp(s, "switch", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    return 6;
                }
            }
            break; 
        case 't':
            if(buflen >= 8) {
                if (!strncmp(s, "typedef", 7) &&
                    !(map[s[7]] & (DIGIT|LETTER))) {
                    return 7; 
                }
            }
            break;
        case 'u':
            if(buflen >= 6) {
                if (!strncmp(s, "union", 5) &&
                    !(map[s[5]] & (DIGIT|LETTER))) {
                    return 5;
                }
            }
            if (buflen >= 9) {
                if (!strncmp(s, "unsigned", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    return 8;
                }
            } 
            break;
        case 'v':
            if(buflen >= 5) {
                if (!strncmp(s, "void", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    return 4; 
                }
            } 
            if (buflen >= 9) {
                if (!strncmp(s, "volatile", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    return 8; 
                }
            }
            break;
        case 'w':
            if(buflen >= 6) {
                if (!strncmp(s, "while", 5) &&
                    !(map[s[5]] & (DIGIT|LETTER))) {
                    return 5; 
                }
            }
            break; 
        case '_':
            if(buflen >= 6) {
                if (!strncmp(s, "_Bool", 5) &&
                    !(map[s[5]] & (DIGIT|LETTER))) {
                    return 5; 
                }

            }
            if (buflen >= 8) {
                if (!strncmp(s, "_Atomic", 7) &&
                    !(map[s[7]] & (DIGIT|LETTER))) {
                    return 7; 
                }

            }
            if (buflen >= 9) {
                if (!strncmp(s, "_Alignas", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    return 8; 
                }
                if (!strncmp(s, "_Alignof", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    return 8;
                }
                if (!strncmp(s, "_Complex", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    return 8;
                }
                if (!strncmp(s, "_Generic", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    return 8;
                }

            }
            if (buflen >= 10) {
                if (!strncmp(s, "_Noreturn", 9) &&
                    !(map[s[9]] & (DIGIT|LETTER))) {
                    return 9; 
                }

            }
            if (buflen >= 11) {
                if (!strncmp(s, "_Imaginary", 10) &&
                    !(map[s[10]] & (DIGIT|LETTER))) {
                    return 10; 
                }

            }
            if (buflen >= 14) {
                if (!strncmp(s, "_Thread_local", 13) &&
                    !(map[s[13]] & (DIGIT|LETTER))) {
                    return 13; 
                }
            }
            if (buflen >= 15) {
                if (!strncmp(s, "_Static_assert", 14) &&
                    !(map[s[14]] & (DIGIT|LETTER))) {
                    return 14; 
                }
            }
            break;
        default: 
            return 0;
    }
    return 0;
}


int is_punct() {
    char *s = buf.curr; 
    int buflen = buf.bufend - buf.curr;
    switch(*s) {
        case '[': case ']': case '(': case ')': case '{': 
        case '}': case '~': case ',': case '?': case ';':
            return 1;
        case '+': 
            if (buflen >= 2 && 
                    (s[1] == '+' || 
                     s[2] == '=')) {
                return 2;
            } 
            return 1;

        case '-':
            if (buflen >= 2 && 
                    (s[1] == '-' ||
                     s[1] == '=' ||
                     s[1] == '>')) {
                return 2; 
            }
            return 1;

        case '<':
            if (buflen >= 3 && 
                    s[1] == '<' && 
                     s[2] == '=')
                return 3; 
            if (buflen >= 2 && 
                    (s[1] == '<' || 
                     s[1] == '=' || 
                     s[1] == ':' ||
                     s[1] == '%'))
                return 2; 
            return 1; 
        
        case '>':
            if (buflen >= 3 && 
                     s[1] == '>' &&
                     s[2] == '=')
                return 3; 

            if (buflen >= 2 && 
                    (s[1] == '>' ||
                     s[1] == '='))
                return 2; 
            return 1;

        case '=':
            if (buflen >= 2 && 
                    s[1] == '=')
                return 2; 
            return 1; 

        case '!':
            if (buflen >= 2 && 
                    s[1] == '=')
                return 2; 
            return 1;

        case '&':
            if (buflen >= 2 && 
                    (s[1] == '&' || 
                    s[1] == '='))
                return 2; 
            return 1; 

        case '|':
            if (buflen >= 2 && 
                    (s[1] == '|' || 
                    s[1] == '='))
                return 2; 
            return 1;

        case '*':
            if (buflen >= 2 && 
                    s[1] == '=')
                return 2; 
            return 1;

        case '/':
            if (buflen >= 2 &&
                    s[1] == '=')
                return 2; 
            return 1;

        case '%':
            if (buflen >= 4 && 
                    s[1] == ':' &&
                    s[2] == '%' &&
                    s[3] == ':')
                return 4; 
            if (buflen >= 2 && 
                    (s[1] == '=' ||
                    s[1] == '>' ||
                    s[1] == ':'))
                return 2;
            return 1;

        case '#':
            if (buflen >= 2 &&
                    s[1] == '#')
                return 2; 
            return 1;

        case ':':
            if (buflen >= 2 &&
                    s[1] == '>')
                return 2;
            return 1;

        case '.':
            if (buflen >= 3 && 
                    s[1] == '.' && 
                    s[2] == '.')
                return 3;
            return 1;
        case '^':
            if (buflen >= 2 && 
                    s[1] == '=') 
                return 2;
            return 1;

        default:
            return 0;
    }
    return 0;

}
