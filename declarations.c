#include "pluto.h"
#include "include/parse.h"

// declarations
Type declaration_specifiers(){
    Token tok;
    // used if declaration is a struct, union or typdefed identifier
    Type t = NULL;
    // collect all specifiers
    struct dec_spec ds = {.scs = 0, .ts = 0, .tq = 0, .fs = 0};
    int seen = 1;
        while (seen) {
            tok = lex();
            // exit loop if FIRST(declarator) or ';'
            if ((tok->type == PUNCT && (tok->subtype == SCOL
                || tok->subtype == LBRAC || tok->subtype == STAR)) ||
                (tok->type == ID && !lookup(tok->val.strval, types)))
                break;
            seen = 0;
            restore_tok(&tok);
            // check if storage class specifier
            if (seen |= storage_class_specifier(&ds))
                continue;
            // check if type specifier
            else if (seen |= type_specifier(&ds, &t))
                continue;
            // check if type qualifier
            else if (seen |= type_qualifier(&ds))
                continue;
            // check if function specifier
            else if (seen |= function_specifier(&ds))
                continue;
            // check if alignment specifier
            else if (seen |= alignment_specifier(&ds))
                continue;
            else
                error(&tok->loc, "type error: expected declaration specifier within declaration");
        }
        // if ds didn't get updated it's an error
        if (!ds.scs && !ds.ts && !ds.tq && !ds.fs) {
            error(&tok->loc, "type error: expected declaration specifier within declaration");
            restore_tok(&tok);
            return inttype;
        }
        restore_tok(&tok);
        return build_type(ds, t);
}



int storage_class_specifier(struct dec_spec *ds){
    Token tok = lex();
    if (tok->type != KEYWORD) {
        restore_tok(&tok);
        return 0;
    }
    // TODO issue error message on violation
    switch (tok->subtype) {
        case TYPEDEF:
            ds->scs |= SCS_TYPEDEF; return 1;
        case EXTERN:
            ds->scs |= SCS_EXTERN; return 1;
        case STATIC:
            ds->scs |= SCS_STATIC; return 1;
        case _THREAD_LOCAL:
            ds->scs |= SCS_THREAD_LOCAL; return 1;
        case AUTO:
            ds->scs |= SCS_AUTO; return 1;
        case REGISTER:
            ds->scs |= SCS_REGISTER; return 1;
    }
    restore_tok(&tok);
    return 0;
}
int type_specifier(struct dec_spec *ds, Type *t){
    Token tok = lex();
    if (tok->type != KEYWORD && tok->type != ID) {
        restore_tok(&tok);
        return 0;
    }

    // typdef name
    if (tok->type == ID) {
        if (*t) {
            error(&tok->loc, "type error: attempting to declare multiple type specifiers");
            *t = NULL;
        }
        Symbol sym = lookup(tok->val.strval, types);
        if (sym == NULL) {
            error(&tok->loc, "type error: identifier doesn't correspond to any valid types");
            *t = inttype;
        } else
            *t = sym->type;
        return 1;
    }
    // TODO issue error message on violation
    switch (tok->subtype) {
        case VOID:
            ds->ts |= TS_VOID; return 1;
        case CHAR:
            ds->ts |= TS_CHAR; return 1;
        case SHORT:
            ds->ts |= TS_SHORT; return 1;
        case INT:
            ds->ts |= TS_INT; return 1;
        case LONG:
            // long previously declared, migrate to long long
            if (ds->ts & TS_LONG) {
                ds->ts &= ~TS_LONG;
                ds->ts |= TS_LLONG;
            } else
                ds->ts |= TS_LONG;
            return 1;
        case FLOAT:
            ds->ts |= TS_FLOAT; return 1;
        case DOUBLE:
            ds->ts |= TS_DOUBLE; return 1;
        case SIGNED:
            ds->ts |= TS_SIGNED; return 1;
        case UNSIGNED:
            ds->ts |= TS_UNSIGNED; return 1;
        case _BOOL:
            ds->ts |= TS_BOOL; return 1;
        case _COMPLEX:
            ds->ts |= TS_COMPLEX; return 1;
        case _ATOMIC:
            // only handle atomic type specifier
            ds->ts |= TQ_ATOMIC; return 1;
        case STRUCT: case UNION:
            if (*t)
                error(&tok->loc, "type error: attempting to make more than only struct declaration");
            restore_tok(&tok);
            *t = struct_or_union_specifier();
            return 1;
        case ENUM:
            ds->ts |= TS_ENUM; return 1;
    }
    restore_tok(&tok);
    return 0;
}
int type_qualifier(struct dec_spec *ds){
    Token tok = lex();
    if (tok->type != KEYWORD) {
        restore_tok(&tok);
        return 0;
    }
    // TODO issue error messages on violation
    switch (tok->subtype) {
        case CONST:
            ds->tq |= TQ_CONST; return 1;
        case RESTRICT:
            error(&tok->loc, "type error: restrict cannot be specified on a non-pointer type");
            return 1;
        case VOLATILE:
            ds->tq |= TQ_VOLATILE; return 1;
        case _ATOMIC:
            ds->tq |= TQ_ATOMIC; return 1;
            break;
    }
    restore_tok(&tok);
    return 0;
}
int function_specifier(struct dec_spec *ds){
    Token tok = lex();
    if (tok->type != KEYWORD) {
        restore_tok(&tok);
        return 0;
    }
    // TODO issue error messages on violation
    switch (tok->subtype) {
        case INLINE:
            ds->fs |= FS_INLINE; return 1;
        case _NORETURN:
            ds->fs |= FS_INLINE; return 1;
    }
    restore_tok(&tok);
    return 0;
}
int alignment_specifier(struct dec_spec *ds){
    Token tok = lex();
    if (tok->type != KEYWORD) {
        restore_tok(&tok);
        return 0;
    }
    // TODO issue error messages on violation
    switch(tok->subtype) {
        case _ALIGNAS:
            // TODO
            return 1;
    }
    restore_tok(&tok);
    return 0;
}

Node init_declarator_list(Type ds){
    // init_declarator
    // init_declarator_list, init_declarator

    Node init_decl = init_declarator(ds);
    if (init_decl->id == ND_FUNC_DEF)
        return init_decl;
    Token tok = lex();
    if (tok->type != PUNCT || tok->subtype != COM) {
        restore_tok(&tok);
        return init_decl;
    }
    restore_tok(&tok);
    Node init_decl_list = make_node(ND_INIT_LIST, PERM);
    add_child(init_decl_list, &init_decl);
    init_decl_list->loc = init_decl->loc;
    while ((tok = lex())->type == PUNCT && tok->subtype == COM) {
        init_decl = init_declarator(ds);
        add_child(init_decl_list, &init_decl);
    }
    restore_tok(&tok);
    return init_decl_list;
}

Node init_declarator(Type ds){
    // declarator
    // declarator = initializer
    Node dd = declarator(ds);
    Token tok = lex();
    // function definition
    if (tok->type == PUNCT && tok->subtype == LCBRAC) {
        return function_definition(ds, dd);
    }
    if (tok->type == PUNCT && tok->subtype == ASSIGN) {
        Node init_decl = make_node(ND_INIT, PERM);
        init_decl->loc = tok->loc;
        add_child(init_decl, &dd);
        Node init = initializer();
        add_child(init_decl, &init);
        return init_decl;
    }

    restore_tok(&tok);
    return dd;
}
Node declarator(Type ds){
    // pointer[opt] direct_declarator
    Token tok = lex();
    // do pointer work
    if (tok->type == PUNCT && tok->subtype == STAR) {}

    restore_tok(&tok);
    // do direct_declarator work
    Node dd = direct_declarator(ds);



}
Node direct_declarator(Type ds){
    // identifier
    // (declarator)
    // direct_declarator [ type_qualifier_list[opt] assignment_expression[opt] ]
    // direct_declarator [ static type_qualifier_list[opt] assignment_expression ]
    // direct_declarator [ type_qualifier_list static assignment_expression ]
    // direct_declarator [ type_qualifier_list[opt] * ]
    // direct_declarator ( parameter_type_list )
    // direct_declarator ( identifier_list[opt] )

    Node dd;
    Token tok = lex();
    if (tok->type == ID) {
        dd = make_node(ND_ID, PERM);
        dd->val.strval = tok->val.strval;
        dd->type = ds;
        dd->loc = tok->loc;

    } else if (tok->type == PUNCT && tok->subtype == LBRAC){
        dd = declarator(ds);

        tok = lex();
        if (tok->type != PUNCT || tok->subtype != RBRAC)
            error(&tok->loc, "missing closing ')' in direct declarator");

    } else if (tok->type != PUNCT || tok->subtype != LBRAC) {
        error(&tok->loc, "expected either identifier or '(' in direct declarator");
    }

    tok = lex();
    while (tok->type == PUNCT && (tok->subtype == LSQBRAC || tok->subtype == LBRAC)) {

        // function declaration
        if (tok->subtype == LSQBRAC) {
            // todo
        } else if (tok->subtype == LBRAC) {
            Node fn_call = make_node(ND_FUNC_CALL, PERM);
            add_child(fn_call, &dd);
            dd = fn_call;

            tok = lex();
            if (tok->type == ID) {
                restore_tok(&tok);
                Node args = identifier_list();
                add_child(fn_call, &args);
            }
            if (tok->type != PUNCT || tok->subtype != RBRAC) {
                restore_tok(&tok);
                error(&tok->loc, "expected either identifier or closing ')' in function call");
            }
        }
    }

    restore_tok(&tok);
    return dd;
}
Node atomic_type_specifier(){}
Type struct_or_union_specifier(){
    Token tok = lex();
    Type su;

    // pick construct
    if (tok->type != KEYWORD || (tok->subtype != STRUCT && tok->subtype != UNION))
        error(&tok->loc, "expected struct or union keyword");
    int id = tok->subtype;
    location l = tok->loc;

    // extract tag
    char *tag = make_string("", 0);
    if ((tok = lex())->type == ID)
        tag = tok->val.strval;
    else
        restore_tok(&tok);

    // If tag wasn't defined and struct declaration list wasn't defined either, error
    if (!(*tag) && (tok = lex())->type != PUNCT || tok->subtype != LBRAC) {
        error(&tok->loc, "expected struct declaration list in struct/union declaration");
        // error recovery work - return dummy struct/union
    }
    su = make_struct(id, tag, &loc);
    tok = lex();

    // struct definition
    if (tok->type == PUNCT && tok->subtype == LCBRAC) {
        su = make_struct(id, tag, &loc);
        // struct declaration list
        su->u.sym->u.s.flist = struct_declaration_list();

        if ((tok = lex())->type != PUNCT || tok->subtype != RCBRAC) {
            error(&tok->loc, "expected '}' at the end of a struct declaration");
            restore_tok(&tok);
        }

    // struct reference
    } else if (*tag) {
        Symbol sym = lookup(tag, types);
        if (!sym || !isstructunion(sym->type)) {
            error(&loc, "type error: provided struct/union tag doesn't correspond to a valid struct/union type");
            // error recovery work - return dummy struct/union
        }
        su = sym->type;
    }

    return su;
}
Node enum_specifier(){}
Field struct_declaration_list(){}
Node struct_declaration(){}
Node specifier_qualifier_list(){}
Node struct_declarator_list(){}
Node struct_declarator(){}
Node enumerator_list(){}
Node enumerator(){}
Node enumeration_constant(){}
Node pointer(){}
Node type_qualifier_list(){}
Node parameter_type_list(){}
Node identifier_list(){}
Node parameter_list(){}
Node parameter_declaration(){}
Node abstract_declarator(){}
Node direct_abstract_declarator(){}
Node initializer(){
    Token tok = lex();
    if (tok->type == PUNCT && tok->subtype == LCBRAC) {
        restore_tok(&tok);
        return initializer_list();
    }
    restore_tok(&tok);
    return assignment_expression();
}
Node initializer_list(){}
Node designation(){}
Node designator_list(){}
Node static_assert_declaration(){
    // _Static_assert ( constant_expression , string-literal ) ;
    Token tok;
    if ((tok = lex())->type != KEYWORD || tok->subtype != _STATIC_ASSERT) {
        error(&(tok->loc), "expected _Static_assert keyword before static assert declaration");
    }
    Node sad = make_node(ND_SAD, PERM);
    sad->loc = tok->loc;
    if ((tok = lex())->type != PUNCT || tok->subtype != LBRAC) {
        error(&(tok->loc), "expected opening '(' in static assert declaration");
    }
    // constant expression
    Node const_ex = constant_expression();
    add_child(sad, &const_ex);
    if ((tok = lex())->type != PUNCT || tok->subtype != COM) {
        error(&(tok->loc), "expected ',' between constant expression and string literal in the static assert declaration");
    }

    // string literal
    if ((tok = lex())->type != STR) {
        // error - expected a string constant
        error(&(tok->loc), "expected string literal as the second argument of the static assert declaration");
    }
    Node str = make_node(ND_STR, PERM);
    str->val.strval = tok->val.strval;
    str->loc = tok->loc;
    add_child(sad, &str);

    // ')'
    if ((tok = lex())->type != PUNCT || tok->subtype != RBRAC) {
        error(&(tok->loc), "expected closing ')' in static assert declaration");
    }
    if ((tok = lex())->type != PUNCT || tok->subtype != SCOL) {
        error((&tok->loc), "missing terminating ';' in static assert declaration");
    }
    return sad;
}

