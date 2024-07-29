#include "pluto.h"
#include "include/charmap.h"


token t;


int lex() {
   // keep lexing till there's no more input in the buffer
    while (buf.curr < buf.bufend) {
        // while the buffer contains characters read them 
        while (buf.curr < buf.bufend) {
            // if buffer contains less then MINBUFLEN characters refill - done to simplify reading
            if ((buf.bufend - buf.curr) <= MINBUFLEN && pp_read_complete) 
                fillbuf();
            
            // extra indirection casue the patterns can be interleaved
            while ((*buf.curr == '#') ||
                    (map[*buf.curr] & BLANK) ||
                    (map[*buf.curr] & NEWLINE)) {
                // TODO process GNU pp directives
                if (*buf.curr == '#') {
                    while (!(map[*buf.curr++] & NEWLINE)) {}
                    loc.x = 0;
                // skip blanks
                } else if (map[*buf.curr] & BLANK) {
                    buf.curr++; 
                    loc.x++; 
                // skip newlines
                } else if (map[*buf.curr] & NEWLINE) {
                    buf.curr++; 
                    loc.x = 0; 
                    loc.y++;
                }
            }

            if (!pp_read_complete && buf.curr == (buf.bufend - 1)) {
                return EOI; 
            }

            int token_len = 0;
            // check if next token is a keyword
            if (token_len = is_string()) {
                t.loc = loc; 
                buf.curr += token_len; 
                loc.x += token_len;
                return t.type;
             // handle illegal input            
            } if (map[*buf.curr] & LETTER) {
                // check if it's a keyword 
                if (token_len = is_keyword()) {
                    // update the global token with the token details
                    t.type = KEYWORD;
                    t.val = make_string(buf.curr, token_len);
                    t.loc = loc;
                    buf.curr += token_len; 
                    loc.x += token_len;
                    return KEYWORD;
                // check if it's an identifier
                } else if (token_len = is_identifier()) {
                    t.type = ID; 
                    t.val = make_string(buf.curr, token_len);
                    t.loc = loc; 
                    buf.curr += token_len; 
                    loc.x += token_len;
                    return ID;
                }
                // if neither, then its an error 
                return make_error();
            } else if (token_len = is_constant()) {
                // token updatation is done internally
                t.val = make_string(buf.curr, token_len);
                t.loc = loc; 
                buf.curr += token_len;
                loc.x += token_len;
                return t.type;

            } else if (token_len = is_punct()) {
                t.type = PUNCT;
                t.val = make_string(buf.curr, token_len);
                t.loc = loc;
                buf.curr += token_len;
                loc.x += token_len;
                return PUNCT;
            } else {
                return make_error();
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
        } else if (tok == PUNCT) {
            fprintf(fp, "PUNCT \"%s\"\n", t.val); 
            fprintf(fp, "Location: %d, %d\n", t.loc.x, t.loc.y);
        } else if (tok == ULLONG) {
            fprintf(fp, "ULLONG \"%s\"\n", t.val); 
            fprintf(fp, "Location: %d, %d\n", t.loc.x, t.loc.y);
        } else if (tok == SLLONG) {
            fprintf(fp, "SLLONG \"%s\"\n", t.val); 
            fprintf(fp, "Location: %d, %d\n", t.loc.x, t.loc.y);
        } else if (tok == ULONG) {
            fprintf(fp, "ULONG \"%s\"\n", t.val); 
            fprintf(fp, "Location: %d, %d\n", t.loc.x, t.loc.y);
        } else if (tok == SLONG) {
            fprintf(fp, "SLONG \"%s\"\n", t.val); 
            fprintf(fp, "Location: %d, %d\n", t.loc.x, t.loc.y);
        } else if (tok == UINT) {
            fprintf(fp, "UINT \"%s\"\n", t.val); 
            fprintf(fp, "Location: %d, %d\n", t.loc.x, t.loc.y);
        } else if (tok == SINT) {
            fprintf(fp, "SINT \"%s\"\n", t.val); 
            fprintf(fp, "Location: %d, %d\n", t.loc.x, t.loc.y);
        } else if (tok == SDOUBLE) {
            fprintf(fp, "SDOUBLE \"%s\"\n", t.val); 
            fprintf(fp, "Location: %d, %d\n", t.loc.x, t.loc.y);
        } else if (tok == LDOUBLE) {
            fprintf(fp, "LDOUBLE \"%s\"\n", t.val); 
            fprintf(fp, "Location: %d, %d\n", t.loc.x, t.loc.y);
        } else if (tok == SFLOAT) {
            fprintf(fp, "SFLOAT \"%s\"\n", t.val); 
            fprintf(fp, "Location: %d, %d\n", t.loc.x, t.loc.y);
        } else if (tok == UCHAR) {
            fprintf(fp, "CHAR '%c'\n", t.charval.c); 
            fprintf(fp, "Location: %d, %d\n", t.loc.x, t.loc.y);
        } else if (tok == STR) {
            fprintf(fp, "STR \"%s\"\n", t.strval); 
            fprintf(fp, "Location: %d, %d\n", t.loc.x, t.loc.y);
        } else if (tok == ERROR) {
            fprintf(fp, "ERROR \"%s\"\n", t.val); 
            fprintf(fp, "Location: %d, %d\n", t.loc.x, t.loc.y);
        } else{
            fprintf(fp, "INTERNAL ERROR!\n");
        }
    }
}

int make_error() {
    char *s = buf.curr; 
    // escape the characters till the next token terminator
    while (!(map[*s] & (BLANK|NEWLINE))) {
        s++;
    }
    t.type = ERROR; 
    t.val = make_string(buf.curr, s - buf.curr);
    t.loc = loc; 
    loc.x += s - buf.curr;
    return ERROR;
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

int is_constant() {
    int token_len = 0; 
    // the precedence of evaluation is determined via the 
    // lazy evaluation of the || operator
    if ((token_len = is_floatconst()) || 
            (token_len = is_intconst()) ||
            (token_len = is_charconst())) {
        return token_len;
    }
    return 0;
}

int is_intconst() {
    char *s = buf.curr;
    int token_len = 0; 
    char unsigned_flag = 0; 
    char long_flag = 0; 
    char llong_flag = 0;
    // assume the largest size and narrow down based on the constants needs
    // constants only represent magnitude, thus ull is the largest container 
    unsigned long long int value = 0;


    // three possibilities (1) hex (2) octal (3) decimal
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        s += 2;
        while((map[s[0]] & DIGIT) || (map[s[0]] & HEX)) {
            value <<= 4;
            if (map[s[0]] & DIGIT) {
                value += (s[0] - 48);
            } else if (map[s[0]] <= 70) {
                value += (s[0] - 55);
            } else {
                value += (s[0] - 87);
            }
            s++;
        }
        if (s == (buf.curr + 2)) {
            // error - need hex digits after hex prefix
            return 0;
        }
    } else if (s[0] == '0') {
        // use s >- 0 && s <= 7
        while ((map[s[0]] & DIGIT) && (s[0] <= 55)) {
            value *= 8; 
            value += (s[0] - 48);
            s++;
        }
    
    // use s >= 1 && s <= 9
    } else if ((map[s[0]] & DIGIT) && s[0] != '0') {
        while (map[s[0]] & DIGIT) {
            value *= 10; 
            value += (s[0] - 48);
            s++; 
        }
    }
    
    // check storage size hint and signedness
    if (s[0] == 'u' || s[0] == 'U') {
        unsigned_flag = 1;
        s++;
        if (s[0] == 'l' || s[0] == 'L') {
            long_flag = 1;
            s++; 
            if (s[0] == 'l' || s[0] == 'L') {
                llong_flag = long_flag--;
                s++;
            }
        }
    } else if (s[0] == 'l' || s[0] == 'L') {
        long_flag = 1; 
        s++; 
        if (s[0] == 'l' || s[0] == 'L') {
            llong_flag = long_flag--;
            s++; 
            if (s[0] == 'u' || s[0] == 'U') {
                unsigned_flag = 1;
                s++;
            }
        }
    }
    token_len = s - buf.curr;
    // based on the value of the intconst & its given size hint and signedness
    // we need allocate the appropriate intconst bucket
    // if the specified value is too large for the largest container, an error is reported 
    // and the value is cast into one that fits in the largest container
    if (unsigned_flag && llong_flag) {

        t.type = ULLONG;  
        t.intval.ull = value;
        if (value > ULLONG_MAX) {
            // error: too big
        }
    } else if (llong_flag) {
        t.type = SLLONG;
        t.intval.ll = (long long int) value;
        if (value > LLONG_MAX) {
            // error: too big
        }
    } else if (unsigned_flag && long_flag) {
        t.type = ULONG;
        t.intval.ul = (unsigned long int) value;
        if (value > ULONG_MAX) {
            t.type = ULLONG;
            t.intval.ull = value;
            if (value > ULLONG_MAX) {
                // error too big
            } 
        }
    } else if (long_flag) {
        t.type = SLONG; 
        t.intval.l = (long int) value; 
        if (value > LONG_MAX) { 
            t.type = SLLONG; 
            t.intval.ll = (long long int) value;
            if (value > LLONG_MAX) {
                // error too big
            } 
        }
    } else if (unsigned_flag) {
        t.type = UINT; 
        t.intval.u = (unsigned int) value; 
        if (value > UINT_MAX) {
            t.type = ULONG; 
            t.intval.ul = (unsigned long int) value; 
            if (value > ULONG_MAX) {
                t.type = ULLONG; 
                t.intval.ull = (unsigned long long int) value; 
                if (value > ULLONG_MAX) {
                    // error too big
                }
            }
        }
    } else {
        t.type = SINT; 
        t.intval.i = (int) value; 
        if (value > INT_MAX) {
            t.type = SLONG; 
            t.intval.l = (long int) value; 
            if (value > LONG_MAX) {
                t.type = SLLONG; 
                t.intval.ll = (long long int) value; 
                if (value > LLONG_MAX) {
                    // error too big 
                }
            }
        }
           
    }
    return token_len;
}

int is_floatconst() {
    char *s = buf.curr;
    int token_len = 0;
    // default is double
    char double_flag = 1;
    // user specifies either of these
    char float_flag = 0; 
    char longdouble_flag = 0;


    // decimal float const
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        s += 2;
        while (map[s[0]] & (HEX | DIGIT)) {
            s++; 
        }
        if (s[0] == '.') {
            s++;
            while (map[s[0]] & (HEX | DIGIT)) {
                s++; 
            }
            if (s[0] == 'p' || s[0] == 'P') {
                s++; 
                if (s[0] == '+' || s[0] == '-') {
                    s++; 
                }
                if (map[s[0]] & DIGIT) {
                    while (map[s[0]] & DIGIT) {
                        s++;
                    }
                } else {
                    // error - required digit 
                }
            }
        } else if (s[0] == 'p' || s[0] == 'P') {
            s++; 
            if (s[0] == '+' || s[0] == '-') {
                s++; 
            }
            if (map[s[0]] & DIGIT) {
                while (map[s[0]] & DIGIT) {
                     s++;
                }
            } else {
                 // error - required digit 
             }

        } else {
            // possibly an int 
            return 0;
        }
        /*
        while (map[s[0]]&DIGIT) {
            s++; 
        }
        */
    } else if (map[s[0]] & DIGIT || s[0] == '.') {
        while (map[s[0]] & DIGIT) {
            s++; 
        }
        if (s[0] == '.') {
            s++;
            while (map[s[0]] & DIGIT) {
                s++; 
            }
            if (s[0] == 'e' || s[0] == 'E') {
                s++; 
                if (s[0] == '+' || s[0] == '-') {
                    s++; 
                }
                if (map[s[0]] & DIGIT) {
                    while (map[s[0]] & DIGIT) {
                        s++;
                    }
                } else {
                    // error - required digit 
                }
            }
        } else if (s[0] == 'e' || s[0] == 'E') {
            s++; 
            if (s[0] == '+' || s[0] == '-') {
                s++; 
            }
            if (map[s[0]] & DIGIT) {
                while (map[s[0]] & DIGIT) {
                     s++;
                }
            } else {
                 // error - required digit 
             }

        } else {
            // possibly an int 
            return 0;
        }
        /*
        while (map[s[0]]&DIGIT) {
            s++; 
        }
        */
    // hex float constant
    }
    // optional floating suffix 
    if (s[0] == 'f' || s[0] == 'F') {
        double_flag = 0; 
        float_flag = 1;
        s++;
    } else if (s[0] == 'l' || s[0] == 'L') {
        double_flag = 0; 
        longdouble_flag = 1;
        s++;
    }
    // use stdlib for the heavy lifting cause who wants to deal with floats

    if (double_flag) {
        t.type = SDOUBLE;
        t.floatval.d = strtod(buf.curr, &s);
        // check overflow 
        if (errno == ERANGE) {
            // error 
        }
    } else if (longdouble_flag) {
        t.type = LDOUBLE;
        t.floatval.ld = strtold(buf.curr, &s); 
        s++;
        // check overflow
        if (errno == ERANGE) {
            // error 
        }
    } else if (float_flag) {
        t.type = SFLOAT;
        t.floatval.f = strtof(buf.curr, &s);
        s++;
        // check overflow 
        if (errno == ERANGE) {
            // error 
        }
    }
    token_len = s - buf.curr;
    return token_len;
}

int is_enumconst() {
    return 0;
}

int is_charconst() {
    char *s = buf.curr; 
    int token_len = 0; 
    char char_flag = 0; 
    int val = 0;
    // TODO: need to add support
    char wchar_flag = 0; 
    char char16_flag = 0; 
    char char32_flag = 0;

    // determine storage size
    if (s[0] == '\'') {
        char_flag = 1; 
        s++;
        // parse contents
        if (s[0] == '\\') {
            int esc_len = is_escapeseq(&val, s);
            if (!esc_len) {
                // error
            }
            s += esc_len;
            if (s[0] != '\'') {
                // error - only single chars allowed 
            }
            s++;
        // not an escape sequence
        } else if (s[1] == '\'') {
            val = s[0];
            s += 2;
        } else {
            // error 
        }
        t.type = UCHAR; 
        t.charval.c = val;
        return (s - buf.curr);
    } else if ((s[0] == 'u') &&
            (s[1] == '\'')) {
        char16_flag = 1;
        s += 2;
        // error - pluto is yet to support
    } else if ((s[0] == 'U') &&
            (s[1] == '\'')) {
        char32_flag = 1;
        s += 2;
        // error - pluto is yet to support
    } else if ((s[0] == 'L') &&
            (s[1] == '\'')) {
        wchar_flag = 1;
        s += 2;
        // error - pluto is yet to support
    } else 
        return 0;
}

int is_escapeseq(int *val, char *s) {
    char *olds = s;

    if (s[0] == '\\') {
        switch (s[1]) {
            case '\'': 
                *val = '\'';
                return 2; 
            case '\"': 
                *val = '\"';
                return 2;
            case '?': 
                *val = '\?';
                return 2;
            case '\\':  
                *val = '\\';
                return 2;
            case 'v': 
                *val = '\v';
                return 2;
            case 'f': 
                *val = '\f';
                return 2;
            case 'r':
                *val = '\r';
                return 2;
            case 't':
                *val = '\t'; 
                return 2;
            case 'n': case '\n':
                *val = '\n';
                return 2;
            case 'a':
                *val = '\a';
                return 2; 
            case 'b':
                *val = '\b';
                return 2;
            // hex 
            case 'x': case 'X':
                s += 2;
                if (!(map[s[0]] & (HEX | DIGIT))) {
                    // error - need a hex digit after \x
                }
                while (map[s[0]] & (HEX | DIGIT)) {
                    (*val) <<= 4; 
                     if (map[s[0]] & DIGIT) {
                        *val += (s[0] - 48);
                    } else if (map[s[0]] <= 70) {
                        *val += (s[0] - 55);
                    } else {
                        *val += (s[0] - 87);
                    }
                    s++;
                }
                // hex
                if (*val > 0xFF) {
                    // error val can't be more than 255 
                }
                return (s - olds);
            // octal
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                s++;
                for (int i = 0; (i < 3) && (map[s[0]] & DIGIT); i++, s++) {
                    if (s[0] == '8' || s[0] == '9') 
                        break;
                    *val << 3;
                    *val += (s[0] - '0');
                }
                return (s - olds);
            default: 
                return 0;
        }
    }
    return 0;
}

int is_string() {
    // ensure buff has at least MAXSTRLEN chars
    if ((buf.bufend - buf.curr) <= MAXSTRLEN && pp_read_complete) 
        fillbuf();

    char *s = buf.curr;
    char strbuf[MAXSTRLEN + 1] = "";
    int escapechar = 0;
    int sbindex = 0;
    // make sure buffer has at least max str len characters
    switch (s[0]) {
        case 'u':
            if (((s[1] == '8') && (s[2] == '"')) ||
                        (s[1] == '"')) {
                // error - pluto doesn't support u8/u
            }
            break;
        case 'U': case 'L':
            if (s[1] == '"') {
                // error - pluto doesn't support U/L
            }
            break;
        case '"':
            s++;
            while(s[0] != '"') {
                if (sbindex > MAXSTRLEN) {
                    // error - str limit is MAXSTRLEN
                    break;
                }
                int esc_len = 0;
                if (esc_len = is_escapeseq(&escapechar, s)) {
                    strbuf[sbindex++] = (char)escapechar;
                    s += esc_len;
                } else {
                    strbuf[sbindex++] = *s; 
                    s++;
                }
            }
            s++;
            strbuf[sbindex] = '\0';
            t.type = STR; 
            t.strval = make_string(strbuf, sbindex);
            return (s - buf.curr);
        default: 
            return 0;
    }
    return 0;
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
