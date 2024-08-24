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
            child = make_node(ND_ID, PERM);
            child->loc = l;
            child->type = ds;
            child->val.strval = dtos(genlabel(1));
            add_child(root, &child);
            continue;
        
        // FIRST(declarator) - '*', identifier, '('
        } else if ((tok->type == PUNCT && (tok->subtype == STAR ||
                        tok->subtype == LBRAC)) || tok->subtype == ID) {
            restore_tok(&tok);
            child = init_declarator_list(ds);
            child->loc = l; 
            add_child(root, &child);
            tok = lex(); 
            if (tok->type != PUNCT || tok->subtype != SCOL)
                error(&tok->loc, "expected ';' after declaration");
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

    // append to the end of ts
    if (!ts) {
        ts = nxt; 
        return; 
    }
    token_store *tscpy = ts;
    while (tscpy->nxt != NULL) 
        tscpy = tscpy->nxt;
    tscpy->nxt = nxt; 
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

    // requires complete redo
    switch (n->id) {
        case ND_ROOT:
            printf("%s \n", node_map[ND_ROOT - 256]);
            break;
        case ND_ID: 
            printf("|-- %s \"%s\" type: %s <line:%d, col:%d>\n", node_map[n->id - 256], \
                    n->val.strval, ttos(n->type), n->loc.y, n->loc.x);
            break;
        case ND_STR:
            printf("|-- %s \"%s\" <line:%d, col:%d>\n", node_map[n->id - 256], \
                    n->val.strval, n->loc.y, n->loc.x);
            break;
        case ND_CONST:
            switch (n->subid) {
                case ND_UCHAR:
                    printf("|-- %s:%s \'%c\' <line:%d, col:%d>\n", node_map[n->id - 256], \
                            node_map[n->subid - 256], n->val.charval.c, n->loc.y, n->loc.x);
                    break; 
                case ND_INT: 
                    switch (n->subsubid) {
                        case ULLONG: 
                            printf("|-- %s:%s:ULLONG %llu <line:%d, col:%d>\n", node_map[ n->id - 256], \
                                   node_map[n->subid - 256], n->val.intval.ull, n->loc.y, n->loc.x);
                            break;
                        case SLLONG: 
                            printf("|-- %s:%s:SLLONG %lld <line:%d, col:%d>\n", node_map[n->id - 256], \
                                   node_map[n->subid - 256], n->val.intval.ll, n->loc.y, n->loc.x);
                            break;
                        case ULONG: 
                            printf("|-- %s:%s:ULONG %lu <line:%d, col:%d>\n", node_map[n->id - 256], \
                                   node_map[n->subid - 256], n->val.intval.ul, n->loc.y, n->loc.x);
                            break;
                        case SLONG: 
                            printf("|-- %s:%s:SLONG %ld <line:%d, col:%d>\n", node_map[n->id - 256], \
                                   node_map[n->subid - 256], n->val.intval.l, n->loc.y, n->loc.x);
                            break;
                        case UINT: 
                            printf("|-- %s:%s:UINT %u <line:%d, col:%d>\n", node_map[n->id - 256], \
                                   node_map[n->subid - 256], n->val.intval.u, n->loc.y, n->loc.x);
                            break;
                        case SINT: 
                            printf("|-- %s:%s:SINT %d <line:%d, col:%d>\n", node_map[n->id - 256], \
                                   node_map[n->subid - 256], n->val.intval.i, n->loc.y, n->loc.x);
                            break; 
                        }
                    break; 
                case ND_FLOAT:
                    switch (n->subsubid) {
                        case SDOUBLE: 
                            printf("|-- %s:%s:SDOUBLE %f <line:%d, col:%d>\n", node_map[n->id - 256], \
                                   node_map[n->subid - 256], n->val.floatval.d, n->loc.y, n->loc.x);
                            break;
                        case LDOUBLE: 
                            printf("|-- %s:%s:LDOUBLE %Lf <line:%d, col:%d>\n", node_map[n->id - 256], \
                                   node_map[n->subid - 256], n->val.floatval.ld, n->loc.y, n->loc.x);
                            break;
                        case SFLOAT: 
                            printf("|-- %s:%s:SFLOAT %f <line:%d, col:%d>\n", node_map[n->id - 256], \
                                   node_map[n->subid - 256], n->val.floatval.f, n->loc.y, n->loc.x);
                            break;
                    }
                    break;
            }
            break;

        case ND_EXPR:



            break;
        case ND_GENERIC: 

            break; 
        case ND_SAD: 
            printf("|-- %s <line:%d, col:%d>\n", node_map[n->id - 256], \
                n->loc.y, n->loc.x);
            break;
        case ND_ASSIGNEXPR: 
            printf("|-- %s:%s <line:%d, col:%d>\n", node_map[n->id - 256], \
                    node_map[n->subid - 256], n->loc.y, n->loc.x);
            break;
        default:
            printf("|-- %s type: %s <line:%d, col:%d>\n", node_map[n->id - 256], \
                    ttos(n->type), n->loc.y, n->loc.x);
            break;
    }
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
                    (tok->type == ID && lookup(tok->val.strval, types)))
                    return 1;
            break;
        case ND_DECL:
            if (tok->type == ID || (tok->type == PUNCT && (tok->subtype == STAR || tok->subtype == LBRAC)))
                return 1;
        default:
            break;
    }
    return 0;
}

