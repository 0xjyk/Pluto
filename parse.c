#include "pluto.h"
#include "include/parse.h"

Node parse(char *pp_file) {
    // initionalise the buffer
    bufinit(pp_file);
    // initialise primary types 
    typeinit();
    // initialise the token store
    ts = NULL;

    // root contains 
    Node root = make_node(ND_ROOT, PERM);
    
    // three options - 
    //  function_definition 
    //      declaration_specifiers declarator declaration_list[opt] compound_statement
    //  declaration 
    //      declaration_specifiers init_declarator_list[opt] ;
    //      static_assert_declaration
    Token tok; 
    Node child;
    while ((tok = lex())->type != EOI) {

        // static_assert_declaration
        if (tok->type == KEYWORD && tok->subtype == _STATIC_ASSERT) {
            restore_tok(&tok);
            child = static_assert_declaration();
            add_child(root, &child);
            continue;
        }
        // declaration_specifiers
        location l = tok->loc;
        restore_tok(&tok);
        Type ds = declaration_specifiers();
        tok = lex();
        Node decl;
        // declaration without optional init_declarator_list
        if (tok->type == PUNCT && tok->subtype == SCOL) {
            // no declarator provided, iterpretted as a type declaration
            child = make_node(ND_TYPE_DECL, PERM);
            child->loc = l;
            child->type = ds;
            //child->val.strval = dtos(genlabel(1));
            add_child(root, &child);
            continue;
        
        // FIRST(declarator) - '*', identifier, '('
        } else if ((tok->type == PUNCT && (tok->subtype == STAR ||
                        tok->subtype == LBRAC)) || tok->type == ID) {
            restore_tok(&tok);
            child = init_declarator_list(ds);
            child->loc = l; 
            //child->type = ds;
            add_child(root, &child);
            if (child->id != ND_FUNC_DEF) {
                tok = lex(); 
                if (tok->subtype != SCOL) {
                    restore_tok(&tok);
                    error(&tok->loc, "expected ';' after declaration");
                }
            }

            continue;
        } else {
            error(&tok->loc, "expected declarator after declaration specifier(s)");
            restore_tok(&tok);
        }
    }
    return root;
}


// helper functions
void print_cpp() {
    while(buf.curr < buf.bufend) {
        while (buf.curr < buf.bufend) {
            fprintf(stdout, "%c", *buf.curr++);
        }
        fillbuf();
    }
}

Node make_node(int id, int arena) {
    Node n = alloc(sizeof(node), arena);
    n->id = id; 
    // n->typ = NULL;
    n->nxt = NULL;
    n->err = 0;
    n->num_kids = 0;
    n->kids = NULL;
    return n; 
}

void add_child(Node n, Node *nw) {
    assert(n != NULL);
    Node nxt = n->kids;
    if (!nxt) {
        n->kids = *nw;
        n->num_kids = 1;
        return;
    }
    while (nxt->nxt != NULL)
        nxt = nxt->nxt;

    nxt->nxt = *nw;
    (*nw)->nxt = NULL;
    n->num_kids++;
}

void restore_tok(Token *tok) {
    token_store *nxt = alloc(sizeof (token_store), PERM); 
    nxt->tok = *tok;
    nxt->nxt = NULL; 

    if (!ts) {
        ts = nxt; 
        return; 
    }
    // insert in the front of the queue
    nxt->nxt = ts;
    ts = nxt;
    /*
    token_store *tscpy = ts;
    while (tscpy->nxt != NULL) 
        tscpy = tscpy->nxt;
    tscpy->nxt = nxt; 
    */
}

void print_node(Node n, int indent) {
    char *node_map[256];
#define NODE(a,b,c) node_map[b - 256] = c;
#include "include/node.h"
#undef NODE
    assert(n != NULL);
    /*
    char idnt[indent + 1];
    memset(idnt, ' ', indent);
    idnt[indent] = '\0';
    printf("%s", idnt);
    */
    for (int i = 1; i < indent; i++) {
        printf("|   ");
    }
#define PCONST(a, b) printf("|-- ND_CONST: [type: %s] \'" a "\' <line:%d, col: %d>\n", \
        ttos(n->type), n->val.b, n->loc.y, n->loc.x)

    // requires complete redo
    switch (n->id) {
    case ND_ROOT:
        printf("%s \n", node_map[ND_ROOT - 256]); break;
    case ND_ID: case ND_STR: case ND_DECL: case ND_FUNC_DEF:
        printf("|-- %s \"%s\" [type: %s] <line:%d, col:%d>\n", node_map[n->id - 256], \
                    n->val.strval, ttos(n->type), n->loc.y, n->loc.x);
        break;
    case ND_CONST:
        switch (n->subid) {
        case ND_UCHAR:      PCONST("%c", charval.c); break;
        case ND_INT: 
            switch (n->subsubid) {
            case ULLONG:    PCONST("%llu", intval.ull); break;
            case SLLONG:    PCONST("%lld", intval.ll); break;
            case ULONG:     PCONST("%lu", intval.ul); break;
            case SLONG:     PCONST("%ld", intval.l); break;
            case UINT:      PCONST("%u", intval.u); break;
            case SINT:      PCONST("%d", intval.i); break;
            }
            break; 
        case ND_FLOAT:
            switch (n->subsubid) {
            case SDOUBLE:   PCONST("%f", floatval.d); break;
            case LDOUBLE:   PCONST("%Lf", floatval.ld); break;
            case SFLOAT:    PCONST("%f", floatval.f); break;
            }
        break;
        }
        break;
    case ND_SAD: case ND_DECL_LIST: case ND_COMPOUND:
        printf("|-- %s <line:%d, col:%d>\n", node_map[n->id - 256], \
                n->loc.y, n->loc.x);
        break;
    default:
        printf("|-- %s [type: %s] <line:%d, col:%d>\n", node_map[n->id - 256], \
                    ttos(n->type), n->loc.y, n->loc.x);
        break;
    }
#undef PCONST
}

void dump_AST(Node n, int indent) {
    if (!n) 
        return;
    // print self
    print_node(n, indent); 
    
    // print children
    Node kid = n->kids;
    while (kid) {
        dump_AST(kid, indent + 1);
        kid = kid->nxt;
    }
}

Node make_error_recovery_node() {
    Token tok = lex(); 
    assert(tok->type == ERROR);
    Node err_nd = make_node(0, PERM); 
    err_nd->err = 1;
    err_nd->loc = tok->loc;
    switch (tok->subtype) {
        case ULLONG: case SLLONG: case ULONG: 
        case SLONG: case UINT: case SINT: 
            err_nd->id = ND_CONST;
            err_nd->subid = ND_INT; 
            err_nd->subsubid = tok->subtype;
            err_nd->val.intval.ull = 0;
            return err_nd;
        case SFLOAT: 
            err_nd->id = ND_CONST;
            err_nd->subid = ND_FLOAT;
            err_nd->subsubid = SFLOAT;
            err_nd->val.floatval.ld = 0.0;
            return err_nd;
        case UCHAR:
            err_nd->id = ND_CONST; 
            err_nd->subid = ND_UCHAR;
            err_nd->val.charval.c = '\0';
            return err_nd;
        case STR: 
            err_nd->id = ND_STR;
            err_nd->val.strval = tok->val.strval;
            return err_nd;
        default: 
            error(&(tok->loc), "internal error while recovering from an error");
            return err_nd;
    }
    return err_nd;
}

void escape_first(int till) {
    Token tok;
    switch(till) {
        case ND_EXPR: ND_ASSIGNEXPR:
            while (1) {
                tok = lex(); 
                if (tok->type == PUNCT && (tok->subtype == LBRAC || tok->subtype == INCR ||
                            tok->subtype == DECR || tok->subtype == BAND || tok->subtype == STAR ||
                            tok->subtype == PLUS || tok->subtype == NEG || tok->subtype == BNOT ||
                            tok->subtype == NOT)) {
                    restore_tok(&tok); 
                    return;
                }
                if (tok->type == KEYWORD && (tok->subtype == SIZEOF || 
                            tok->subtype == _ALIGNOF || tok->subtype == _GENERIC)) {
                    restore_tok(&tok); 
                    return; 
                }
            }
        default: 
            return;
    }
}

_Bool first(int id, Token tok) {
    switch(id) {
        case ND_DS: 
            if ((tok->type == KEYWORD && tok->subtype >= CHAR && tok->subtype <= _NORETURN) || 
                    tok->subtype == _BOOL || (tok->type == ID && typedef_name(tok)))
                    return 1;
            break;
        case ND_DECL:
            if (tok->type == ID || (tok->type == PUNCT && (tok->subtype == STAR || tok->subtype == LBRAC)))
                return 1;
            break;
        case ND_TYPENAME:
            if ((tok->type == KEYWORD && tok->subtype >= CHAR && tok->subtype <= _ATOMIC) || 
                    tok->subtype == _BOOL || (tok->type == ID && typedef_name(tok)))
                    return 1;
            break;
        case ND_UNARY: case ND_EXPR:
            switch (tok->type) {
                case ID: case STR: case UCHAR: case INTCONST: case ERROR:
                    // a typedef name shouldn't begin an expression 
                    // except if it is function
                    if (tok->type == ID && typedef_name(tok)) {
                        return 0;
                    }
                    return 1;
                    break;
                case PUNCT: 
                    switch(tok->subtype) {
                        case LBRAC: case LCBRAC: case INCR: case DECR: case BAND: 
                        case STAR: case PLUS: case NEG: case BNOT: case NOT:
                            return 1;
                        default: 
                            return 0;
                    }
                case KEYWORD:
                    switch(tok->subtype) {
                        case SIZEOF: case _ALIGNOF: case _GENERIC:
                            return 1;
                        default:
                            return 0;
                    }
                default: 
                    return 0;

            }
        default:
            break;
    }
    return 0;
}

