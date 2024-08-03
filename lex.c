/*
    file: lex.c
    description: lex.c provides lexical capabilites to pluto. In it's current state, 
    lex can be used by the parser to retrieve the next token. Alternatively, 
    it can be used by the lex driver to create a *.ll" file with the lexical output.
    implementation notes: 

    limitation: 
        (1) lex, and thereby pluto doesn't support UTF8, wide chars, 16 & 32 bit chars.
            this means that lex only supports ascii chars and strings
            

    author: jasraj kripalani
*/


#include "pluto.h"
#include "include/charmap.h"

// forward declarations
Token is_identifier();
Token is_constant();
Token is_intconst();
Token is_floatconst(); 
Token is_enumconst();
Token is_charconst();
Token is_string();
Token is_keyword();
Token is_punct();
Token make_token(token tok);
int is_escapeseq(int *val, char *s);
void process_gnu_directive();


/*

*/
Token lex() {
    // refill the buffer when the number of characters are too few 
    if ((buf.bufend - buf.curr) <= MINBUFLEN && !pp_read_complete) {
        fillbuf();
    }

    if (pp_read_complete && buf.curr == (buf.bufend - 1)) {
        token tok = {.type=EOI, .len=0, .loc=loc};
        return make_token(tok);
    }

    
    // extra indirection cause the patterns can be interleaved
    while ((*buf.curr == '#') ||
            (map[*buf.curr] & BLANK) ||
            (map[*buf.curr] & NEWLINE)) {
        if (*buf.curr == '#') {
            process_gnu_directive();
        // skip blanks and update file coordinates
        } else if (map[*buf.curr] & BLANK) {
            buf.curr++; 
            loc.x++;
        // skip newlines and update file coordinates
        } else if (map[*buf.curr] & NEWLINE) {
            buf.curr++; 
            loc.x = 0;
            loc.y++; 
        }
    }
    if (pp_read_complete && buf.curr == (buf.bufend - 1)) {
        token tok = {.type=EOI, .len=0, .loc=loc};
        return make_token(tok);
    }
    Token t = NULL;
    
    // flag char prefixes and string prefixs as errors, skip char/string body
    if ((*buf.curr == 'u') || (*buf.curr == 'U') || (*buf.curr == 'L')) {
        char *s = buf.curr;
        if (buf.curr[1] == '\'') {
            // pluto doesn't support char16_t, char32_t & wchar_t constants
            s += 2;
            while (*s != '\'') {
                s++;
            }
            s++;
            error(&loc, "pluto doesn't support char16_t, char32_t & wchar_t character constants");
            token tok = {.type=ERROR, .len=(s-buf.curr), .loc=loc};
            tok.val.strval = make_string(buf.curr, tok.len);
            return make_token(tok);
        } else if (buf.curr[1] == '\"') {
            // pluto doesn't support u, U, L string constants
            ensure_buflen(STRBUFLEN);
            s = buf.curr + 2;
            while (*s != '\"') {
                s++; 
            }
            s++;
            error(&loc, "pluto doesn't support char16_t, char32_t & wchar_t string constants");
            token tok = {.type=ERROR, .len=(s-buf.curr), .loc=loc};
            tok.val.strval = make_string(buf.curr, tok.len);
            return make_token(tok);
        } else if ((buf.curr[0] == 'u') && (buf.curr[1] == '8') 
                && (buf.curr[2] == '\"')) {
            // plutdo doesn't support u8 string constants
            ensure_buflen(STRBUFLEN);
            s = buf.curr + 3;
            while (*s != '\"') {
                s++; 
            }
            s++;
            error(&loc, "pluto doesn't support UTF-8 string constants");
            token tok = {.type=ERROR, .len=(s-buf.curr), .loc=loc};
            tok.val.strval = make_string(buf.curr, tok.len);
            return make_token(tok);
        }
    }
    if (*buf.curr == '\"' && (t = is_string())) {
        return t;
    } else if ((map[*buf.curr] & LETTER) && (t = is_keyword())) {
        return t;
    } else if ((map[*buf.curr] & LETTER) && (t = is_identifier())) {
        return t;
    } else if (t = is_punct()) {
        return t;
    } else if (t = is_constant()) {
        return t;
    // error - didn't match any lexical elements
    } else { 
        token tok = {.type=ERROR, .len=1, .loc=loc};
        tok.val.strval = make_string("unknown character", 17);
        return make_token(tok);
    }
     
}


void lexdriver(char *pp_file) {
    // initialise the buffer to facilitate reading
    bufinit(pp_file);

    // create the output file
    char lex_file[PATH_MAX] = "";
    strcat(lex_file, pp_file);
    lex_file[strlen(lex_file)-2] = 'l';
    lex_file[strlen(lex_file)-1] = 'l';
    FILE *fp = fopen(lex_file, "w");
    if (fp == NULL) {
        printf("couldn't open %s\n", lex_file);
        exit(EXIT_FAILURE);
    }

    char *tok_map[256];
#define TOKEN(a,b,c) tok_map[b] = c;
#define KEY(a,b,c) tok_map[b] = c;
#include "include/token.h"
#undef TOKEN
#undef KEY
    
    Token tok = NULL;
    while (1) {
        tok = lex();
        if (tok == NULL) {
            fprintf(fp, "INTERNAL ERROR!\n");
            fprintf(fp, "location: %d, %d\n", tok->loc.x, tok->loc.y);
            continue;
        }
        switch (tok->type) {
            case ID: 
                fprintf(fp, "ID \"%s\"\n", tok->val.strval); break;
            case KEYWORD: 
                fprintf(fp, "KEYWORD: %s \"%s\"\n", tok_map[tok->subtype], tok->val.strval); break;
            case PUNCT:
                fprintf(fp, "PUNCT: %s \"%s\"\n", tok_map[tok->subtype], tok->val.strval); break;
            case UCHAR:
                fprintf(fp, "UCHAR \'%c\'\n", tok->val.charval.c); break;
            case STR: 
                fprintf(fp, "STR \"%s\"\n", tok->val.strval); break;
            case INTCONST:
                switch (tok->subtype) {
                    case ULLONG: 
                        fprintf(fp, "INTCONST: %s %llu\n", tok_map[tok->subtype], tok->val.intval.ull);
                        break;
                    case SLLONG:
                        fprintf(fp, "INTCONST: %s %lld\n", tok_map[tok->subtype], tok->val.intval.ll);
                        break;
                    case ULONG:
                        fprintf(fp, "INTCONST: %s %lu\n", tok_map[tok->subtype], tok->val.intval.ul);
                        break;
                    case SLONG:
                        fprintf(fp, "INTCONST: %s %ld\n", tok_map[tok->subtype], tok->val.intval.l);
                        break;
                    case UINT:
                        fprintf(fp, "INTCONST: %s %u\n", tok_map[tok->subtype], tok->val.intval.u);
                        break;
                    case SINT:
                        fprintf(fp, "INTCONST: %s %d\n", tok_map[tok->subtype], tok->val.intval.i);
                        break;
                }
                break;
            case FLOATCONST:
                switch (tok->subtype) {
                    case SDOUBLE:
                        fprintf(fp, "FLOATCONST: %s %f\n", tok_map[tok->subtype], tok->val.floatval.d);
                        break;
                    case LDOUBLE:
                        fprintf(fp, "FLOATCONST: %s %Lf\n", tok_map[tok->subtype], tok->val.floatval.ld);
                        break;
                    case SFLOAT:
                        fprintf(fp, "FLOATCONST: %s %f\n", tok_map[tok->subtype], tok->val.floatval.f);
                        break;
                }
                break;
            case ERROR: 
                fprintf(fp, "ERROR \"%s\"\n", tok->val.strval); break;
            case EOI:
                fprintf(fp, "EOI\n"); 
                fprintf(fp, "location: %d, %d\n", tok->loc.x, tok->loc.y); return;
            default: 
                fprintf(fp, "INTERNAL ERROR: lex driver shouldn't print this message\n"); 
                return;
        }
        fprintf(fp, "location: %d, %d\n", tok->loc.x, tok->loc.y);
    }
}


Token is_identifier() {
    ensure_buflen(MINBUFLEN);
    char *s = buf.curr;
    // since lex checks that buf.curr points to a letter
    // we are assured that the loop below atleast runs once
    while (map[*s] & (DIGIT | LETTER)) {
        s++;
        // EOI terminates identifier 
        if (pp_read_complete && s == (buf.bufend - 1)) {
            break;
        }
    }
    // anything that's not a digit or a letter terminates id 
    token tok = {.type=ID, .len=(s-buf.curr), .loc=loc};
    tok.val.strval = make_string(buf.curr, (s - buf.curr));
    Token t = make_token(tok);
    return t;
}

Token is_constant() {
    Token t = NULL;
    if ((t = is_floatconst()) || 
            (t = is_intconst()) ||
            (t = is_charconst())) {
        return t;
    }
    return NULL;
}


Token is_intconst() {
    char *s = buf.curr;
    int err = 0;
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
            } else if (s[0] <= 70) {
                value += (s[0] - 55);
            } else {
                value += (s[0] - 87);
            }
            s++;
        }
        if (s == (buf.curr + 2)) {
            // error - need hex digits after hex prefix
            err = 1;
            error(&loc, "hexadecimal integer constant requires hex digits after 0x");
            token tok = {.type=ERROR, .subtype=SINT, .len=(s-buf.curr), .loc=loc};
            tok.val.strval = make_string(buf.curr, (s - buf.curr));
            return make_token(tok);
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
    } else {
        return NULL;
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
    token tok = {.type=INTCONST, .len=(s-buf.curr), .loc=loc};
    // based on the value of the intconst & its given size hint and signedness
    // we need allocate the appropriate intconst bucket
    // if the specified value is too large for the largest container, an error is reported
    // and the value is cast into one that fits in the largest container
    if (unsigned_flag && llong_flag) {

        tok.subtype = ULLONG;
        tok.val.intval.ull = value;
        if (value > ULLONG_MAX) {
            tok.type = ERROR;
            tok.subtype = ULLONG;
            tok.val.strval = make_string(buf.curr, s - buf.curr);
            error(&loc, "int const (unsigned long long) larger than ULLONG_MAX");
        }
    } else if (llong_flag) {
        tok.subtype = SLLONG;
        tok.val.intval.ll = (long long int) value;
        if (value > LLONG_MAX) {
            tok.type = ERROR;
            tok.subtype = SLLONG;
            tok.val.strval = make_string(buf.curr, s - buf.curr);
            error(&loc, "int const (signed long long) larger than LLONG_MAX");
        }
    } else if (unsigned_flag && long_flag) {
        tok.subtype = ULONG;
        tok.val.intval.ul = (unsigned long int) value;
        if (value > ULONG_MAX) {
            tok.subtype = ULLONG;
            tok.val.intval.ull = value;
            if (value > ULLONG_MAX) {
                // error too big
                tok.type = ERROR;
                tok.subtype = ULLONG;
                tok.val.strval = make_string(buf.curr, s - buf.curr);
                error(&loc, "int const (unsigned long long) larger than ULLONG_MAX");
            }
        }
    } else if (long_flag) {
        tok.subtype = SLONG;
        tok.val.intval.l = (long int) value;
        if (value > LONG_MAX) {
            tok.subtype = SLLONG;
            tok.val.intval.ll = (long long int) value;
            if (value > LLONG_MAX) {
                // error too big
                tok.type = ERROR;
                tok.subtype = SLLONG;
                tok.val.strval = make_string(buf.curr, s - buf.curr);
                error(&loc, "int const (signed long long) larger than LLONG_MAX");
            }
        }
    } else if (unsigned_flag) {
        tok.subtype = UINT;
        tok.val.intval.u = (unsigned int) value;
        if (value > UINT_MAX) {
            tok.subtype = ULONG;
            tok.val.intval.ul = (unsigned long int) value;
            if (value > ULONG_MAX) {
                tok.subtype = ULLONG;
                tok.val.intval.ull = (unsigned long long int) value;
                if (value > ULLONG_MAX) {
                    // error too big
                    tok.type = ERROR;
                    tok.subtype = ULLONG;
                    tok.val.strval = make_string(buf.curr, s - buf.curr);
                    error(&loc, "int const (unsigned long long) larger than ULLONG_MAX");
                }
            }
        }
    } else {
        tok.subtype = SINT;
        tok.val.intval.i = (int) value;
        if (value > INT_MAX) {
            tok.subtype = SLONG;
            tok.val.intval.l = (long int) value;
            if (value > LONG_MAX) {
                tok.subtype = SLLONG;
                tok.val.intval.ll = (long long int) value;
                if (value > LLONG_MAX) {
                    // error too big
                    tok.type = ERROR;
                    tok.subtype = SLLONG;
                    tok.val.strval = make_string(buf.curr, s - buf.curr);
                    error(&loc, "int const (signed long long) larger than LLONG_MAX");
                }
            }
        }

    }
    return make_token(tok);
}

Token is_floatconst() {
    char *s = buf.curr;
    // default is double
    char double_flag = 1;
    // user specifies either of these
    char float_flag = 0;
    char longdouble_flag = 0;
    char err = 0;


    // hex/octal float const
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
                    err = 1;
                    error(&loc, "hex/octal float const expected digit after p/P");
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
                err = 1;
                error(&loc, "hex/octal float const expected digit after p/P");
             }

        } else {
            // possibly an int
            return NULL;
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
                    err = 1;
                    error(&loc, "hex/octal float const expected digit after e/E");
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
                err = 1;
                error(&loc, "hex/octal float const expected digit after e/E");
             }

        } else {
            // possibly an int
            return NULL;
        }
        /*
        while (map[s[0]]&DIGIT) {
            s++;
        }
        */
    } else {
        return NULL;
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
    if (err) {
        token tok = {.type=ERROR, .subtype=SFLOAT, .len=(s-buf.curr), .loc=loc};
        tok.val.strval = make_string(buf.curr, (s - buf.curr));
        return make_token(tok);
    }
    // use stdlib for the heavy lifting cause who wants to deal with floats
    token tok = {.type=FLOATCONST, .loc=loc};
    if (double_flag) {
        tok.subtype = SDOUBLE;
        tok.val.floatval.d = strtod(buf.curr, &s);
        // check overflow
        if (errno == ERANGE) {
            errno = 0;
            err = 1;
            error(&loc, "float const (double) exceeds max limit");
        }
    } else if (longdouble_flag) {
        tok.subtype = LDOUBLE;
        tok.val.floatval.ld = strtold(buf.curr, &s);
        s++;
        // check overflow
        if (errno == ERANGE) {
            errno = 0;
            // error
            err = 1;
            error(&loc, "float const (long double) exceeds max limit");
        }
    } else if (float_flag) {
        tok.subtype = SFLOAT;
        tok.val.floatval.f = strtof(buf.curr, &s);
        s++;
        // check overflow
        if (errno == ERANGE) {
            errno = 0;
            // error
            err = 1;
            error(&loc, "float const (signed float) exceeds max limit");
        }
    }
    if (err) {
        // subtype not accurate, but that's okay
        token tok = {.type=ERROR, .subtype=SFLOAT, .len=(s-buf.curr), .loc=loc};
        tok.val.strval = make_string(buf.curr, (s - buf.curr));
        return make_token(tok);
    }
    tok.len = s - buf.curr;
    return make_token(tok);
}


Token is_charconst() {
    if (*buf.curr != '\'') {
        return NULL;
    }
    ensure_buflen(MINBUFLEN);
    char *s = buf.curr + 1;
    int val = 0;
    char err = 0;
    // parse char contents
    if (*s == '\\') {
        int esc_len = is_escapeseq(&val, s);
        if (!esc_len) {
            // error - escape sequence requires atleast one character
            err = 1;
            error(&loc, "escape sequence requires atleast one valid escape character after \\");
            val = 0;
            esc_len = 1;
        }
        s += esc_len;
    } else {
        val = *s++;
    }
    if (*s++ != '\'') {
        err = 1;
        error(&loc, "In it's current implementation, pluto only supports single character constants");

        // for error recovery skip characters till the closing single quote
        while (*s != '\'') {
            s++;
            ensure_buflen(MINBUFLEN);
            if (s == (buf.bufend - 1)) {
                err = 1;
                error(&loc, "unterminated character constant");
            }
        }
    }
    if (err) {
        token tok = {.type=ERROR, .subtype=UCHAR, .len=(s-buf.curr), .loc=loc};
        tok.val.strval = make_string(buf.curr, (s - buf.curr));
        return make_token(tok);
    }
    token tok = {.type=UCHAR, .len=(s-buf.curr), .loc=loc};
    tok.val.charval.c = val;
    return make_token(tok);
}

int is_escapeseq(int *val, char *s) {
    *val = 0;
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
                    // for error recovery, set the value to 0
                    *val = 0;
                    error(&loc, "a hexadecimal escape sequence requires atleast one hex digit (0-F)");
                    return (s - olds);

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
                if ((*val) > 0xFF) {
                    // error val can't be more than 255
                    // for error recovery, set the value to 255
                    *val = 0xFF; 
                    error(&loc, "A hexadecimal escape sequence cannot have a value greater than 0xFF");
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

Token is_string() {
    ensure_buflen(STRBUFLEN); 
    char *s = buf.curr; 
    char strbuf[MAXSTRLEN + 1] = "";
    int escapechar = 0; 
    int sbindex = 0;
    // make sure buffer has atleast max str len characters
    if (*s++ != '\"') {
        return NULL;
    }
    while (*s != '\"') {
        if (sbindex >= MAXSTRLEN) {
            error(&loc, "string too long. Max supported string length is 512 characters");
            // skip characters till closing "
            while(*s != '\"') {
                s++;
                ensure_buflen(STRBUFLEN);
            }
            s++;
            // return error token
            token tok = {.type=ERROR, .subtype=STR, .len = (s - buf.curr), .loc=loc}; 
            tok.val.strval = make_string(buf.curr, tok.len);
            return make_token(tok);
        }
        int esc_len = 0;
        if (*s == '\\' && (esc_len = is_escapeseq(&escapechar, s))) {
            strbuf[sbindex++] = (char)escapechar;
            s += esc_len;
        } else {
            strbuf[sbindex++] = *s;
            s++;
        }
    }
    s++; 
    strbuf[sbindex] = '\0';
    token tok = {.type=STR, .len=(s - buf.curr), .loc=loc};
    tok.val.strval = make_string(strbuf, sbindex);
    return make_token(tok);
}


Token is_keyword() {
    ensure_buflen(MINBUFLEN);
    char *s = buf.curr; 
    int buflen = buf.bufend - buf.curr;
    token tok;
    switch (*s) {
        case 'a':
            if (buflen >= 5) {
                if (!strncmp(s, "auto", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=AUTO, .len=4, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 4);
                    return make_token(tok);
                }
            }
            break;
        case 'b':
            if (buflen >= 6) {
                if (!strncmp(s, "break", 5) &&
                    !(map[s[5]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=BREAK, .len=5, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 5);
                    return make_token(tok);
                }
            }
            break;
        case 'c':
            if (buflen >= 5) {
                if (!strncmp(s, "case", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=CASE, .len=4, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 4);
                    return make_token(tok);
                }
                if (!strncmp(s, "char", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=CHAR, .len=4, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 4);
                    return make_token(tok);
                }
            }
            if (buflen >= 6) {
                if (!strncmp(s, "const", 5) &&
                    !(map[s[5]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=CONST, .len=5, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 5);
                    return make_token(tok);
                }
            }
            break;
        case 'd':
            if (buflen >= 3) {
                if (!strncmp(s, "do", 2) &&
                    !(map[s[2]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=DO, .len=2, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }
            if (buflen >= 7) {
                if (!strncmp(s, "double", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=DOUBLE, .len=6, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 6);
                    return make_token(tok);
                }
            }
            if (buflen >= 8) {
                if (!strncmp(s, "default", 7) &&
                    !(map[s[7]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=DEFAULT, .len=7, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 7);
                    return make_token(tok);
                }
            }
            break;
        case 'e':
            if (buflen >= 5) {
                if (!strncmp(s, "else", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=ELSE, .len=4, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 4);
                    return make_token(tok);
                }
                if (!strncmp(s, "enum", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=ENUM, .len=4, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 4);
                    return make_token(tok);
                }
            }
            if (buflen >= 7) {
                if (!strncmp(s, "extern", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=EXTERN, .len=6, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 6);
                    return make_token(tok);
                }
            }
            break;
        case 'f':
            if (buflen >= 4) {
                if (!strncmp(s, "for", 3) &&
                    !(map[s[3]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=FOR, .len=3, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 3);
                    return make_token(tok);
                }
            }

            if (buflen >= 6) {
                if (!strncmp(s, "float", 5) &&
                    !(map[s[5]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=FLOAT, .len=5, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 5);
                    return make_token(tok);
                }
            }
            break;
        case 'g':
            if(buflen >= 5) {
                if (!strncmp(s, "goto", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=GOTO, .len=4, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 4);
                    return make_token(tok);
                }
            }
            break;
        case 'i':
            if(buflen >= 3) {
                if (!strncmp(s, "if", 2) &&
                    !(map[s[2]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=IF, .len=2, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }
            if (buflen >= 4) {
                if (!strncmp(s, "int", 3) &&
                    !(map[s[3]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=INT, .len=3, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 3);
                    return make_token(tok);
                }
            }
            if(buflen >= 7) {
                if (!strncmp(s, "inline", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=INLINE, .len=6, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 6);
                    return make_token(tok);
                }
            }
            break;
        case 'l':
            if(buflen >= 5) {
                if (!strncmp(s, "long", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=LONG, .len=4, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 4);
                    return make_token(tok);
                }
            }
            break;
        case 'r':
            if(buflen >= 7) {
                if (!strncmp(s, "return", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=RETURN, .len=6, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 6);
                    return make_token(tok);
                }
            }
            if (buflen >= 9) {
                if (!strncmp(s, "register", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=REGISTER, .len=8, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 8);
                    return make_token(tok);
                }
                if (!strncmp(s, "restrict", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=RESTRICT, .len=8, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 8);
                    return make_token(tok);
                }
            }
            break;
        case 's':
            if (buflen >= 6) {
                if (!strncmp(s, "short", 5) &&
                    !(map[s[5]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=SHORT, .len=5, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 5);
                    return make_token(tok);
                }
            }
            if(buflen >= 7) {
                if (!strncmp(s, "signed", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=SIGNED, .len=6, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 6);
                    return make_token(tok);
                }
                if (!strncmp(s, "sizeof", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=SIZEOF, .len=6, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 6);
                    return make_token(tok);
                }
                if (!strncmp(s, "static", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=STATIC, .len=6, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 6);
                    return make_token(tok);
                }
                if (!strncmp(s, "struct", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=STRUCT, .len=6, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 6);
                    return make_token(tok);
                }
                if (!strncmp(s, "switch", 6) &&
                    !(map[s[6]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=SWITCH, .len=6, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 6);
                    return make_token(tok);
                }
            }
            break;
        case 't':
            if(buflen >= 8) {
                if (!strncmp(s, "typedef", 7) &&
                    !(map[s[7]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=TYPEDEF, .len=7, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 7);
                    return make_token(tok);
                }
            }
            break;
        case 'u':
            if(buflen >= 6) {
                if (!strncmp(s, "union", 5) &&
                    !(map[s[5]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=UNION, .len=5, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 5);
                    return make_token(tok);
                }
            }
            if (buflen >= 9) {
                if (!strncmp(s, "unsigned", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=UNSIGNED, .len=8, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 8);
                    return make_token(tok);
                }
            }
            break;
        case 'v':
            if(buflen >= 5) {
                if (!strncmp(s, "void", 4) &&
                    !(map[s[4]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=VOID, .len=4, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 4);
                    return make_token(tok);
                }
            }
            if (buflen >= 9) {
                if (!strncmp(s, "volatile", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=VOLATILE, .len=8, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 8);
                    return make_token(tok);
                }
            }
            break;
        case 'w':
            if(buflen >= 6) {
                if (!strncmp(s, "while", 5) &&
                    !(map[s[5]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=WHILE, .len=5, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 5);
                    return make_token(tok);
                }
            }
            break;
        case '_':
            if(buflen >= 6) {
                if (!strncmp(s, "_Bool", 5) &&
                    !(map[s[5]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=_BOOL, .len=5, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 5);
                    return make_token(tok);
                }

            }
            if (buflen >= 8) {
                if (!strncmp(s, "_Atomic", 7) &&
                    !(map[s[7]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=_ATOMIC, .len=7, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 7);
                    return make_token(tok);
                }

            }
            if (buflen >= 9) {
                if (!strncmp(s, "_Alignas", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=_ALIGNAS, .len=8, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 8);
                    return make_token(tok);
                }
                if (!strncmp(s, "_Alignof", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=_ALIGNOF, .len=8, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 8);
                    return make_token(tok);
                }
                if (!strncmp(s, "_Complex", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=_COMPLEX, .len=8, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 8);
                    return make_token(tok);
                }
                if (!strncmp(s, "_Generic", 8) &&
                    !(map[s[8]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=_GENERIC, .len=8, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 8);
                    return make_token(tok);
                }

            }
            if (buflen >= 10) {
                if (!strncmp(s, "_Noreturn", 9) &&
                    !(map[s[9]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=_NORETURN, .len=9, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 9);
                    return make_token(tok);
                }

            }
            if (buflen >= 11) {
                if (!strncmp(s, "_Imaginary", 10) &&
                    !(map[s[10]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=_IMAGINARY, .len=10, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 10);
                    return make_token(tok);
                }

            }
            if (buflen >= 14) {
                if (!strncmp(s, "_Thread_local", 13) &&
                    !(map[s[13]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=_THREAD_LOCAL, .len=13, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 13);
                    return make_token(tok);
                }
            }
            if (buflen >= 15) {
                if (!strncmp(s, "_Static_assert", 14) &&
                    !(map[s[14]] & (DIGIT|LETTER))) {
                    token tok = {.type=KEYWORD, .subtype=_STATIC_ASSERT, .len=14, .loc = loc};
                    tok.val.strval = make_string(buf.curr, 14);
                    return make_token(tok);
                }
            }
            break;
        default:
            return NULL;
    }
    return NULL;
}

// probably the stupidest function i've ever written
/*


*/
Token is_punct() {
    ensure_buflen(3);
    char *s = buf.curr;
    int buflen = buf.bufend - buf.curr;
    switch(*s) {
        case '[': case ']': case '(': case ')': case '{':
        case '}': case '~': case ',': case '?': case ';':
        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '+': 
            if (buflen >= 2) {
                // ++
                if (s[1] == '+') {
                    token tok = {.type=PUNCT, .subtype=INCR, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                // +=
                } else if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=INCREQ, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }
            // + 
        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '-':
            if (buflen >= 2) {
                // --
                if (s[1] == '-') {
                    token tok = {.type=PUNCT, .subtype=DECR, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                // -= 
                } else if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=DECREQ, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                // ->
                } else if (s[1] == '>') {
                    token tok = {.type=PUNCT, .subtype=ARROW, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }
            // -
        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '<':
            if (buflen >= 3) {
                // <<=
                if (s[1] == '<' && s[2] == '=') {
                    token tok = {.type=PUNCT, .subtype=LSHFTEQ, .len=3, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 3);
                    return make_token(tok);
                }
            } 
            if (buflen >= 2) {
                // << 
                if (s[1] == '<') {
                    token tok = {.type=PUNCT, .subtype=LSHFT, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                // <= 
                } else if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=LEQ, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                // <:
                } else if (s[1] == ':') {
                    token tok = {.type=PUNCT, .subtype=LSQBRACREP, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                // <%
                } else if (s[1] == '%') {
                    token tok = {.type=PUNCT, .subtype=LCBRACREP, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }
            // <
        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '>':
            if (buflen >= 3) {
                // >>=
                if (s[1] == '>' && s[2] == '=') {
                    token tok = {.type=PUNCT, .subtype=RSHFTEQ, .len=3, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 3);
                    return make_token(tok);
                }
            }
            if (buflen >= 2) {
                // >>
                if (s[1] == '>') {
                    token tok = {.type=PUNCT, .subtype=RSHFT, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                // >=
                } else if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=GEQ, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }
            // >
        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '=':
            if (buflen >= 2) {
                // ==
                if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=EQ, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2); 
                    return make_token(tok);
                }
            }
            // =
        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc}; 
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '!': 
            if (buflen >= 2) {
                if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=NEQ, .len=2, .loc=loc}; 
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }
            // !
        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '&': 
            if (buflen >= 2) {
                // && 
                if (s[1] == '&') {
                    token tok = {.type=PUNCT, .subtype=AND, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2); 
                    return make_token(tok);
                // &=
                } else if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=ANDEQ, .len=2, .loc=loc}; 
                    tok.val.strval = make_string(buf.curr, 2); 
                    return make_token(tok);
                }
            }
        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '|': 
            if (buflen >= 2) {
                // ||
                if (s[1] == '|') {
                    token tok = {.type=PUNCT, .subtype=OR, .len=2, .loc=loc}; 
                    tok.val.strval = make_string(buf.curr, 2); 
                    return make_token(tok);
                // |=
                } else if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=OREQ, .len=2, .loc=loc}; 
                    tok.val.strval = make_string(buf.curr, 2); 
                    return make_token(tok);
                }
            }
            // |
        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc}; 
            tok.val.strval = make_string(buf.curr, 1); 
            return make_token(tok);
        }
        case '*':
            if (buflen >= 2) {
                // *= 
                if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=MULTEQ, .len=2, .loc=loc}; 
                    tok.val.strval = make_string(buf.curr, 2); 
                    return make_token(tok); 
                }
            }
            // *
        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1); 
            return make_token(tok); 
        }
        case '/': 
            if (buflen >= 2) {
                // /=
                if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=DIVEQ, .len=2, .loc=loc}; 
                    tok.val.strval = make_string(buf.curr, 2); 
                    return make_token(tok); 
                }
            }
            // /
        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc}; 
            tok.val.strval = make_string(buf.curr, 1); 
            return make_token(tok);
        }
        case '%':
            if (buflen >= 4) {
                // %:%: i.e. ##
                if (s[1] == ':' && s[2] == '%' && s[3] == ':') {
                    token tok = {.type=PUNCT, .subtype=HHASHREP, .len=4, .loc=loc}; 
                    tok.val.strval = make_string(buf.curr, 4); 
                    return make_token(tok);
                }
            }
            if (buflen >= 2) {
                // %=
                if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=MODEQ, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                // %> 
                } else if (s[1] == '>') {
                    token tok = {.type=PUNCT, .subtype=RCBRACREP, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                // %:
                } else if (s[1] == ':') {
                    token tok = {.type=PUNCT, .subtype=HASHREP, .len=2, .loc=loc}; 
                    tok.val.strval = make_string(buf.curr, 2); 
                    return make_token(tok);
                }
            }
            // %
        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc}; 
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '#':
            if (buflen >= 2) {
                // ##
                if (s[1] == '#') {
                    token tok = {.type=PUNCT, .subtype=HHASH, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }
            // #
        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc}; 
            tok.val.strval = make_string(buf.curr, 1); 
            return make_token(tok);
        }
        case ':':
            if (buflen >= 2) {
                // :>
                if (s[1] == '>') {
                    token tok = {.type=PUNCT, .subtype=RSQBRACREP, .len=2, .loc=loc}; 
                    tok.val.strval = make_string(buf.curr, 1); 
                    return make_token(tok); 
                }
            }
            // :
        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc}; 
            tok.val.strval = make_string(buf.curr, 1); 
            return make_token(tok); 
        }
        case '.': 
            if (buflen >= 3) {
                // ...
                if (s[1] == '.' && s[2] == '.') {
                    token tok = {.type=PUNCT, .subtype=ELLIPS, .len=3, .loc=loc}; 
                    tok.val.strval = make_string(buf.curr, 3); 
                    return make_token(tok); 
                }
            }
            // .
        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        }
        case '^':
            if (buflen >= 2) {
                // ^= 
                if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=XOREQ, .len=2, .loc=loc}; 
                    tok.val.strval = make_string(buf.curr, 2); 
                    return make_token(tok); 
                }
            }
            // ^ 
        {
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1); 
            return make_token(tok);
        }
        default: 
            return NULL;
    }
    return NULL; 
}

void process_gnu_directive() {
    ensure_buflen(STRBUFLEN);
    
    char *s = buf.curr;
    // skip directive prefix
    if (*buf.curr == '#') {
        buf.curr += 2;
    }
    // parse line number
    Token tok = NULL;
    tok = is_intconst();
    if ((tok != NULL) && (tok->type == INTCONST)) {
        buf.curr++;
        loc.y = tok->val.intval.i;
        loc.x = 0;
        // set all gnu flags as off by default
        loc.nf = 0; loc.rtf = 0; loc.shf = 0; loc.tae = 0;
    } else {
        error(&loc, "internal error while processing gnu cpp directive");
    }
    // parse file name
    tok = NULL;
    tok = is_string();
    if ((tok != NULL) && (tok->type == STR)) {
        loc.file = tok->val.strval;
    } else {
        error(&loc, "internal error while processing gnu cpp directive");
    }
    
    // next character should be '\n' is not optional gnu flags are present
    if (*buf.curr++ == '\n') {
        return;
    }
    // parse optional gnu flags 
    if (*buf.curr == '1') {
        loc.nf = 1;
        buf.curr += 2;
    }
    if (*buf.curr == '2') {
        loc.rtf = 1;
        buf.curr += 2;
    }
    if (*buf.curr == '3') {
        loc.shf = 1;
        buf.curr += 2;
    } 
    if (*buf.curr == '4') {
        loc.tae = 1;
        buf.curr += 1;
    }
    
    while (*buf.curr++ != '\n') {
    }
}

/*
    make_token returns a heap allocated copy
    of tok and increments buf.curr & loc appropriately
*/
Token make_token(token tok) {
    Token result = alloc(sizeof(tok), PERM);
    *result = tok;

    buf.curr += tok.len;
    loc.x += tok.len;

    return result; 
}
