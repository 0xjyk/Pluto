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
Token is_contants();
Token is_intconst();
Token is_floatconst(); 
Token is_enumconst();
Token is_charconst();
Token is_string();
Token is_keyword();
Token is_punct();
Token make_token(token tok);
Token make_error();
int is_escapeseq(int *val, char *s);
void process_gnu_directives();


/*

*/
Token lex() {
    // refill the buffer when the number of characters are too few 
    if ((buf.bufend - buf.curr) <= MINBUFLEN && !pp_read_complete) {
        fillbuf();
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
            return make_token(tok);
        }
    }
    if (*buf.curr == '\"' && t = is_string()) {
        return t;
    } else if ((map[*buf.curr] & LETTER) && t = is_keyword()) {
        return t;
    } else if ((map[*buf.curr] & LETTER) && t = is_identifier()) {
        return t;
    } else if (t = is_constant()) {
        return t;
    } else if (t = is_punct()) {
        return t;
    }
    // error - didn't match any lexical elements
     
}


void lexdriver(char *pp_file) {

}

Token make_error() {
    
    return ERROR;
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

Token is_contants() {
    Token t = NULL;
    if ((t = is_floatconst()) || 
            (t = is_intconst()) ||
            (t = is_char_const())) {
        return t;
    }
    return NULL;
}

Token is_intconst() {

}

Token is_floatconst() {

}

Token is_enumconst() {

}

Token is_charconst() {

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

Token is_string() {
    ensure_buflen(STRBUFLEN); 
    char *s = buf.curr; 
    char strbuf[MAXSTRLEN + 1] = "";
    int escapechar = 0; 
    int sbindex = 0;
    // make sure buffer has atleast max str len characters
    if (*s++ == '\"') {
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
            return make_token(tok);
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
    token tok = {.type=STR, .len=(s - buf.curr), .loc=loc};
    tok.val.strval = make_string(strbuf, sbindex);
    return make_token(tok):
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
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
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
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        case '-':
            if (buflen >= 2) {
                // --
                if (s[1] == '-') {
                    token tok = {.type=PUNCT, .subtype=DECR, .len=2, .loc=loc};
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                // -= 
                } else if (s[1] == '=') {
                    token tok = {.type=PUNCt, .subtype=DECREQ, .len=2, .loc=loc};
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
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        case '<'
            if (buflen >= 3) {
                // <<=
                if (s[1] == '<' && s[2] == '=') {
                    token tok = {.type=PUNCT, .subtype=LSFTEQ, .len=3, .loc=loc};
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
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
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
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
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
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc}; 
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        case '!': 
            if (buflen >= 2) {
                if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=NEQ, .len=2, .loc=loc}; 
                    tok.val.strval = make_string(buf.curr, 2);
                    return make_token(tok);
                }
            }
            // !
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
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
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
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
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc}; 
            tok.val.strval = make_string(buf.curr, 1); 
            return make_token(tok);
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
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1); 
            return make_token(tok); 
        case '/': 
            if (buflen >= 2) {
                // /=
                if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=DIVEQ, .len=2, .loc=2}; 
                    tok.val.strval = make_string(buf.curr, 2); 
                    return make_token(tok); 
                }
            }
            // /
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc}; 
            tok.val.strval = make_string(buf.curr, 1); 
            return make_token(tok);
        case '%':
            if (buflen >= 4) {
                // %:%: i.e. ##
                if (s[1] == ':' && s[2] == '%' && s[3] == ':') {
                    token tok = {.type=PUNCT, .subtype=HHASHREP, .len=4, .loc=loc}; 
                    tok.val.strval = make_sting(buf.curr, 4); 
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
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc}; 
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
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
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc}; 
            tok.val.strval = make_string(buf.curr, 1); 
            return make_token(tok);
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
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc}; 
            tok.val.strval = make_string(buf.curr, 1); 
            return make_token(tok); 
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
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1);
            return make_token(tok);
        case '^':
            if (buflen >= 2) {
                // ^= 
                if (s[1] == '=') {
                    token tok = {.type=PUNCT, .subtype=XOREQ, .len=2. .loc=loc}; 
                    tok.val.strval = make_sting(buf.curr, 2); 
                    return make_token(tok); 
                }
            }
            // ^ 
            token tok = {.type=PUNCT, .subtype=*s, .len=1, .loc=loc};
            tok.val.strval = make_string(buf.curr, 1); 
            return make_token(tok);
        default: 
            return NULL;
    }
    return NULL; 
}

void process_gnu_directive() {
    // TODO process GNU pp direcitve
    while (!(map[*buf.curr] & NEWLINE)) {
        buf.curr++;
    }
    loc.x = 0;
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
