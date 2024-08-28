#include "pluto.h"
#include "include/parse.h"

// declarations
Type declaration_specifiers(){
    // storage_class_specifiers declarations_specifiers[opt]
    // type_specifiers declarations_specifiers[opt]
    // type_qualifiers declarations_specifiers[opt]
    // function_specifiers declarations_specifiers[opt]
    // alignment_specifiers declarations_specifiers[opt]
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
                || tok->subtype == LBRAC || tok->subtype == STAR ||
                tok->subtype == COM || tok->subtype == RBRAC)) ||
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
        if (!(t || ds.scs || ds.ts || ds.tq || ds.fs)) {
            error(&tok->loc, "type error: expected declaration specifier within declaration");
            restore_tok(&tok);
            return sinttype;
        }
        restore_tok(&tok);
        return build_type(ds, t);
}



int storage_class_specifier(struct dec_spec *ds){
    // typedef | extern | static | _Thread_local | auto | register
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
    // void | char | short | int | long | float
    // double | signed | unsigned | _Bool | _Complex
    // atomic_type_specifier
    // struct_or_union_specifier
    // enum_specifier
    // typedef_name
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
            *t = sinttype;
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
    if (!*tag) {
        tag = dtos(genlabel(1));
        if (((tok = lex())->type != PUNCT || tok->subtype != LCBRAC)) {
            error(&tok->loc, "expected struct declaration list in struct/union declaration");
            // error recovery work - return dummy struct/union
            return make_struct(id, tag, &loc);
        }
        restore_tok(&tok);
    }
    
    su = make_struct(id, tag, &loc);
    tok = lex();

    // struct definition
    if (tok->type == PUNCT && tok->subtype == LCBRAC) {
        // struct declaration list
        su->u.sym->u.s.flist = struct_declaration_list(su);

        if ((tok = lex())->type != PUNCT || tok->subtype != RCBRAC) {
            error(&tok->loc, "expected '}' at the end of a struct declaration");
            restore_tok(&tok);
        }

    } else 
        restore_tok(&tok);
    return su;
}

Field struct_declaration_list(Type su){
    // struct_declaration
    // struct_declaration_list struct_declaration
    Field fd = NULL, ffd;

    Token tok;
    while((tok = lex())->type != PUNCT || tok->subtype != RCBRAC) {
        restore_tok(&tok);
        if (!fd) {
            fd = struct_declaration(su);
            ffd = fd;
        } else {
            fd->next = struct_declaration(su);
        }
        // struct declaration could possibly
        // create multiple fields, move fd to the last field
        while (fd->next)
            fd = fd->next;
    }
    fd = ffd;
    if (!fd) {
        error(&tok->loc, "struct declaration list expects atleast one field");
        // make dummy field for error recovery
        fd = make_field(dtos(genlabel(1)), su, sinttype);
    }
    restore_tok(&tok);
    return fd;
}
Field struct_declaration(Type su){
    // specifier_qualifier_list struct_declaration_list[opt] ;
    // static_assert_declaration
    Token tok;
    // do static_assert work

    Type sql = specifier_qualifier_list(su);
    char *name = make_string("", 0);
    Field fd = NULL, ffd;

    // struct_declarator_list
    // struct_declarator
    // struct_declartor_list , struct_declarator
    tok = lex();
    if (tok->type != PUNCT || tok->subtype != SCOL) {
        // struct_declarator
        do {
            if (!fd) {
                restore_tok(&tok);
                fd = struct_declarator(su, sql);
                ffd = fd;
            } else 
                fd->next = struct_declarator(su, sql);
            if (fd->next)
                fd = fd->next;
        } while ((tok = lex())->type == PUNCT && tok->subtype == COM);
        if (tok->type != PUNCT || tok->subtype != SCOL) {
            error(&tok->loc, "expected ';' after struct declaration");
        }
    } else
        return make_field(dtos(genlabel(1)), su, sql);
    return ffd;
}
Type specifier_qualifier_list() {
    // type_specifier specifier_qualifier_list[opt]
    // type_qualifer specifier_qualifier_list[opt]
    struct dec_spec ds = {.scs = 0, .ts = 0, .tq = 0, .fs = 0};
    int seen = 1;
    Type t = NULL;
    Token tok;
    while (seen) {
        tok = lex(); 
        // exit if not a type specifier or type qualifier
        if ((tok->type == ID && !lookup(tok->val.strval, types)) ||
            (tok->type == PUNCT && (tok->subtype == COL || 
            tok->subtype == SCOL || tok->subtype == STAR)))
            break;

        seen == 0;
        restore_tok(&tok);
        if (seen |= type_specifier(&ds, &t))
            continue;
        if (seen |= type_qualifier(&ds))
            continue;
    }
    if (!(t || ds.ts || ds.tq)) {
        error(&tok->loc, "type error: expected type specifier or type qualifier in struct member declaration");
        restore_tok(&tok);
        return sinttype;
    }
    restore_tok(&tok);
    return build_type(ds, t);
}

Field struct_declarator(Type su, Type sql){
    // declarator
    // declarator[opt] : constant_expression
    Token tok = lex();
    Symbol sym = NULL;
    if (first(ND_DECL, tok)) {
        restore_tok(&tok);
        sym = declarator(sql, 0);
        tok = lex();
    }
    if (!sym) {
        sym = alloc(sizeof(symbol), PERM);
        sym->name = dtos(genlabel(1));
        sym->type = sql;
    }

    // convert sym to field
    Field fd = make_field(sym->name, su, sym->type);

    // define bit width if declaration contains bit width
    if (tok->type == PUNCT && tok->subtype == COL) {
        tok = lex(); 
        if (tok->type != INTCONST && tok->type != UCHAR) {
            restore_tok(&tok);
            error(&tok->loc, "expected interger constant expression in struct bit-width specifier");
        } else 
            // incorrect, fix later
            fd->bitsize = tok->val.charval.c;
    } else 
        restore_tok(&tok);
    return fd;
}
Node enum_specifier(){
    // enum identifier[opt] { emunerator_list }
    // enum identifier[opt] { enumerator_list , }
    // enum identifier
}
Node enumerator_list(){
    // enumerator
    // enumerator_list , enumerator
}
Node enumerator(){
    // enumeration_constant
    // enumeration_constant = constant_expression
}
Node atomic_type_specifier(){
    // _Atomic ( type_name )
}

int type_qualifier(struct dec_spec *ds){
    // const | restrict | volatile | _Atomic
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
    // inline | _Noreturn
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
    // _Alignas ( type_name )
    // _Alignas ( constant_expression )
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

    Node dd = make_node(ND_INIT, PERM);
    dd->sym = declarator(ds, 0);
    dd->loc = dd->sym->loc;
    dd->type = dd->sym->type;
    Token tok = lex();
    // function definition
    if (tok->type == PUNCT && tok->subtype == LCBRAC) {
        restore_tok(&tok);
        return function_definition(dd->sym);
    }
    if (tok->type == PUNCT && tok->subtype == ASSIGN) {
        dd->loc = tok->loc; 
        Node init = initializer();
        add_child(dd, &init);
        return dd;
    }
    restore_tok(&tok);
    return dd;
}
Symbol declarator(Type ds, _Bool dad){
    // pointer[opt] direct_declarator
    Token tok = lex();
    if (tok->type == PUNCT && tok->subtype == STAR) {
        restore_tok(&tok);
        ds = pointer(ds);
    } else
        restore_tok(&tok);
    // do direct_declarator work
    return direct_declarator(ds, dad);
}

Symbol direct_declarator(Type ds, _Bool dad){
    // identifier
    // (declarator)
    // direct_declarator [ type_qualifier_list[opt] assignment_expression[opt] ]
    // direct_declarator [ static type_qualifier_list[opt] assignment_expression ]
    // direct_declarator [ type_qualifier_list static assignment_expression ]
    // direct_declarator [ type_qualifier_list[opt] * ]
    // direct_declarator ( parameter_type_list )
    // direct_declarator ( identifier_list[opt] )
    Symbol sym  = alloc(sizeof(symbol), PERM);
    Token tok = lex(); 
    if (tok->type == ID) {
        sym->name = tok->val.strval;
        sym->loc = tok->loc;
        dad = 0;
    } else if (tok->type == PUNCT && tok->subtype == LBRAC) {
        // since the declarator is enclosed with brackets, don't influence it's type yet
        sym = declarator(NULL, dad);
        tok = lex(); 
        if (tok->type != PUNCT || tok->subtype != RBRAC) {
            error(&tok->loc, "missing closing ')' in direct declarator");
            restore_tok(&tok);
        }
    } else if (dad) {
        sym->name = dtos(genlabel(1));
        sym->type = ds;
        sym->loc = tok->loc;
        restore_tok(&tok);

    } else {
        sym->name = dtos(genlabel(1)); 
        sym->loc = tok->loc;
        error(&tok->loc, "expected either identifier or '(' in direct declarator");
        restore_tok(&tok);
    }
    tok = lex(); 
    // not a function or array declaration
    if (tok->type != PUNCT || (tok->subtype != LSQBRAC && tok->subtype != LBRAC)) {
        restore_tok(&tok);
        sym->type = ds;
        return sym;
    }
    restore_tok(&tok);
    // extract array / function type 
    Type decl = func_or_array_decl(ds);
    if (sym->type == NULL)
        sym->type = decl;
    else if (isptr(sym->type)) {
        if (isqual(sym->type))
            sym->type = qual(sym->type->id, make_ptr(decl));
        else 
            sym->type = make_ptr(decl);
    } else if (isfunc(sym->type)) {
        // decl is the return type
        if (sym->type->type == NULL) {
            sym->type->type == decl;
        } else if (isptr(sym->type->type)) {
            if (isqual(sym->type->type))
                sym->type->type = qual(sym->type->type->id, make_ptr(decl));
            else 
                sym->type->type = make_ptr(decl);
        } else 
            // error ?
            error(&tok->loc, "internal error encountered while building function declaration");
    } else if (isarray(sym->type)) {
        
    }
    return sym;
}

Type pointer(Type ds){
    // * type_qualifier_list[opt]
    // * type_qualifier_list[opt] pointer
    Token tok = lex();
    if (tok->type != PUNCT || tok->subtype != STAR) {
        restore_tok(&tok);
        return ds;
    }
    ds = make_ptr(ds);
    _Bool brk = 1;
    while (brk) {
       tok = lex();
       switch(tok->subtype) {
           case CONST: case RESTRICT:
           case VOLATILE: case _ATOMIC:
               restore_tok(&tok);
               ds = type_qualifier_list(ds);
               break;
           case STAR:
               ds = make_ptr(ds);
               break;
           default:
               brk = 0;
               restore_tok(&tok);
       }
    }
    return ds;
}
Type type_qualifier_list(Type ds){
    // type_qualifier
    // type_qualifier_list type_qualifier
    Token tok;
    Type tq;
    unsigned short tqmap = 0;
    _Bool brk = 1;
    while (brk) {
        tok = lex();
        switch(tok->subtype) {
            case CONST:
                tqmap |= TQ_CONST; break;
            case RESTRICT:
                tqmap |= TQ_RESTRICT; break;
            case VOLATILE:
                tqmap |= TQ_VOLATILE; break;
            case _ATOMIC:
                tqmap |= TQ_ATOMIC; break;
            default:
                restore_tok(&tok);
                brk = 0;
        }
    }
    return qual(tqmap, ds);
}

Type func_or_array_decl(Type ds) {
    Token tok = lex(); 
    if (tok->type == PUNCT && tok->subtype == LSQBRAC) {
        // get array declaration
        restore_tok(&tok);
        Type arr = array_decl(ds);
        // recursively get subtype
        Type arr_child = func_or_array_decl(ds);
        // update array with more precise type and size
        arr->type = arr_child;
        arr->size = arr->size * arr_child->size;
        return arr;
    }
    if (tok->type == PUNCT && tok->subtype == LBRAC) {
        restore_tok(&tok);
        Vector proto = func_decl();
        return make_func(ds, proto);
    }
    restore_tok(&tok);
    return ds;
}

Type array_decl(Type ds){
    // [ type_qualifier_list[opt] assignment_expression[opt] ]
    // [ static type_qualifier_list[opt] assignment_expression ]
    // [ type_qualifier_list static assignment_expression ]
    // [ type_qualifier_list[opt] *]
    Token tok = lex();
    if (tok->type != PUNCT || tok->subtype != LSQBRAC) {
        error(&tok->loc, "expected '[' in array declaration");
        restore_tok(&tok);
    }
    tok = lex();
    Type arr;
    // variable length array
    if (tok->type == PUNCT && tok->subtype == RSQBRAC) {
        return make_array(ds, 0, 0);
    } else if (tok->type == INTCONST)
        arr = make_array(ds, tok->val.intval.u, 0);
    else  {
        restore_tok(&tok);
        // do assignment expression work here
    }
    tok = lex();
    if (tok->type != PUNCT && tok->subtype != RSQBRAC) {
        error(&tok->loc, "expected ']' in array declaration");
    }
    return arr;
}

Vector func_decl(){
    // ( parameter_type_list ) 
    Token tok = lex(); 
    if (tok->type != PUNCT && tok->subtype != LBRAC) {
        error(&tok->loc, "expected '(' before function declaration");
        restore_tok(&tok);
    }
    Vector proto;
    tok = lex(); 
    // empty parameter list 
    if (tok->type == PUNCT && tok->subtype == RBRAC) {
        proto = vec_init(2);
        Symbol sym = alloc(sizeof(symbol), PERM);
        sym->name = dtos(genlabel(1));
        sym->type = voidtype;
        vec_pushback(proto, sym);
        vec_pushback(proto, NULL);
        return proto;
    }
    restore_tok(&tok);
    proto = parameter_list();
    tok = lex(); 
    if (tok->type != PUNCT && tok->subtype != RBRAC) {
        error(&tok->loc, "expected ')' after function declaration");
        restore_tok(&tok);
    }
    return proto;
}

// returns a Vector of Symbols
Vector parameter_list() {
    // parameter_declaration
    // parameter_list , parameter_declaration
    Token tok;
    Vector params = vec_init(2);
    Symbol param = parameter_declaration(); 
    vec_pushback(params, param);
    while ((tok = lex())->type == PUNCT && tok->subtype == COM) {
        tok = lex(); 
        if (tok->type == PUNCT && tok->subtype == ELLIPS) {
            Symbol sym = alloc(sizeof(symbol), PERM);
            sym->name = dtos(genlabel(1));
            sym->type = voidtype;
            vec_pushback(params, sym);
            vec_pushback(params, NULL);
            return params;
        }
        restore_tok(&tok);
        param = parameter_declaration();
        vec_pushback(params, param);
    }
    restore_tok(&tok);
    vec_pushback(params, NULL);
    return params;
}

Symbol parameter_declaration() {
    // declaration_specifiers declarator
    // declaration_specifiers abstract_declarator[opt]
    Type ds = declaration_specifiers();
    Symbol param; 
    Token tok = lex(); 
    // incorrect todo 
    if (first(ND_DECL, tok)) {
        restore_tok(&tok);
        param = declarator(ds, 1);
    } else {
        param = alloc(sizeof(symbol), PERM);
        param->name = dtos(genlabel(1));
        param->type = ds;
        restore_tok(&tok);
    }
    return param;
}
/*
Symbol parameter_declaration() {
    // declaration_specifiers declarator
    // declaration_specifiers abstract_declarator[opt]
    Type ds = declaration_specifiers();
    Symbol param; 
    Token tok = lex(); 
    if (tok->type == ID) {
        restore_tok(&tok);
        param = declarator(ds);
        return param;
    }
    switch(tok->subtype) {
        case COM: case RBRAC:
            restore_tok(&tok);
            param = alloc(sizeof(symbol), PERM); 
            param->name = dtos(genlabel(1));
            param->type = ds;
            break;
        case STAR: 

        case LBRAC:
    }
    return param;

}
*/

Node identifier_list(){
    // identifier
    // identifier , identifier
}
Node type_name(){
    // specifier_qualifier_list abstract_declarator[opt]
}
Node abstract_declarator(){
    // pointer
    // pointer[opt] direct_abstract_declarator
}
Node direct_abstract_declarator(){
    // ( abstract_declarator )
    // direct_abstract_declarator[opt] [ type_qualifier_list[opt] assignment_expression[opt] ]
    // direct_abstract_declarator[opt] [ static type_qualifier_list[opt] assignment_expression ]
    // direct_abstract_declarator[opt] [ type_qualifier_list[opt] static assignment_expression ]
    // direct_abstract_declarator[opt] [ * ]
    // direct_abstract_declarator[opt] ( parameter_type_list[opt] )
}
Node initializer(){
    // assignment_expression
    // { initializer_list }
    // { initializer_list , }
    Token tok = lex();
    if (tok->type == PUNCT && tok->subtype == LCBRAC) {
        restore_tok(&tok);
        return initializer_list();
    }
    restore_tok(&tok);
    return assignment_expression();
}
Node initializer_list(){
    // designation[opt] initializer
    // initializer_list , designation[opt] initializer
    Node init_list = make_node(ND_INIT_LIST, PERM);
    Node init = initializer_list_helper();
    add_child(init_list, &init);
    Token tok;
    while ((tok = lex())->type == PUNCT && tok->subtype == COM) {
        tok = lex();
        if (tok->type == PUNCT && tok->subtype == RCBRAC) {
            restore_tok(&tok);
            return init_list;
        }
        restore_tok(&tok);
        init = initializer_list_helper();
        add_child(init_list, &init);
    }
    restore_tok(&tok);
    return init_list;
}

Node initializer_list_helper() {
    // designation[opt] initializer
    Token tok = lex();
    Node init = make_node(ND_INIT, PERM);
    init->loc = tok->loc;
    if (tok->type == PUNCT && (tok->subtype == LSQBRAC || tok->subtype == DOT)) {
        restore_tok(&tok);
        Node desig = designation();
        add_child(init, &desig);
    }
    Node init_child = initializer();
    add_child(init, &init_child);
    return init;
}
Node designation(){
    // designator_list =
    Node desig_list = designator_list();
    Token tok = lex();
    if (tok->type != PUNCT || tok->type != ASSIGN) {
        error(&tok->loc, "expected '=' sign after designator list in initializer");
        restore_tok(&tok);
    }
    return desig_list;
}
Node designator_list(){
    // designator
    // designator_list designator
    Node desig_list = make_node(ND_DESIG_LIST, PERM);
    Node desig = designator();
    add_child(desig_list, &desig);
    Token tok = lex();
    desig_list->loc = tok->loc;
    while (tok->type = PUNCT && (tok->subtype == LSQBRAC || tok->subtype == DOT)) {
        restore_tok(&tok);
        desig = designator();
        add_child(desig_list, &desig);
        tok = lex();
    }
    restore_tok(&tok);
    return desig_list;
}
Node designator(){
    // [ constant_expression ]
    // . identifier
    Token tok = lex();
    if (tok->type == PUNCT && tok->subtype == LSQBRAC) {
        Node sel = make_node(ND_SELECT, PERM);
        sel->loc = tok->loc;
        Node const_expr = constant_expression();
        add_child(sel, &const_expr);
        tok = lex();
        if (tok->type != PUNCT || tok->type != RSQBRAC) {
            error(&tok->loc, "expected closing ']' in  designator");
            restore_tok(&tok);
        }
        return sel;
    }
    if (tok->type != PUNCT || tok->subtype != DOT) {
        error(&tok->loc, "expected designator in designator list");
    }
    Node dot = make_node(ND_DOT, PERM);
    dot->loc = tok->loc;
    Node id = make_node(ND_ID, PERM);
    tok = lex();
    if (tok->type != ID) {
        error(&tok->loc, "expected identifier after '.' in designator");
        id->val.strval = dtos(genlabel(1));
        restore_tok(&tok);
        return dot;
    }
    id->val.strval = tok->val.strval;
    id->loc = tok->loc;
    add_child(dot, &id);
    return dot;
}
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
    str->type = charptype;
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

Type build_type(struct dec_spec ds, Type ty) {
    // build type
    Type dec_type = NULL;
    // add type qualifiers, if specified
    if (ds.tq & TQ_ATOMIC)
       dec_type = make_type(_ATOMIC, NULL, 0, 0, NULL, make_string("_Atomic", 6));
    else if (ds.tq == TQ_CONST)
        dec_type = make_type(CONST, NULL, 0, 0, NULL, make_string("const", 5));
    else if (ds.tq == TQ_VOLATILE)
        dec_type = make_type(CONST, NULL, 0, 0, NULL, make_string("volatile", 8));
    else if (ds.tq == (TQ_CONST + TQ_VOLATILE))
        dec_type = make_type(CONST + VOLATILE, NULL, 0, 0, NULL, make_string("const volatile", 14));
    // add type specifier
    Type *t = (dec_type ? &dec_type->type : &dec_type);
    if (ty) {
        *t = ty;
    } else if (ds.ts == TS_VOID)
        *t = voidtype;
    else if ((ds.ts == TS_BOOL) ||
            (ds.ts == TS_SIGNED + TS_BOOL) ||
            (ds.ts == TS_UNSIGNED + TS_BOOL))
        *t = booltype;
    else if (ds.ts == TS_CHAR)
        *t = chartype;
    else if (ds.ts == TS_SIGNED + TS_CHAR)
        *t = schartype;
    else if (ds.ts == TS_UNSIGNED + TS_CHAR)
        *t = uchartype;
    else if ((ds.ts == TS_SHORT) ||
            (ds.ts == TS_SIGNED + TS_SHORT) ||
            (ds.ts == TS_SHORT + TS_INT) ||
            (ds.ts == TS_SIGNED + TS_SHORT + TS_INT))
        *t = sshorttype;
    else if ((ds.ts == TS_UNSIGNED + TS_SHORT) ||
            (ds.ts == TS_UNSIGNED + TS_SHORT + TS_INT))
        *t = ushorttype;
    else if ((ds.ts == TS_INT) ||
            (ds.ts == TS_SIGNED) ||
            (ds.ts == TS_SIGNED + TS_INT))
        *t = sinttype;
    else if ((ds.ts == TS_UNSIGNED) ||
            (ds.ts == TS_UNSIGNED + TS_INT))
        *t = uinttype;
    else if ((ds.ts == TS_LONG) ||
            (ds.ts == TS_SIGNED + TS_LONG) ||
            (ds.ts == TS_LONG + TS_INT) ||
            (ds.ts == TS_SIGNED + TS_LONG + TS_INT))
        *t = slongtype;
    else if ((ds.ts == TS_UNSIGNED + TS_LONG) ||
            (ds.ts == TS_UNSIGNED + TS_LONG + TS_INT))
        *t = ulongtype;
    else if ((ds.ts == TS_LLONG) ||
            (ds.ts == TS_SIGNED + TS_LLONG) ||
            (ds.ts == TS_LLONG + TS_INT) ||
            (ds.ts == TS_SIGNED + TS_LLONG + TS_INT))
        *t = slonglongtype;
    else if ((ds.ts == TS_UNSIGNED + TS_LLONG) ||
            (ds.ts == TS_UNSIGNED + TS_LLONG + TS_INT))
        *t = ulonglongtype;
    else if (ds.ts == TS_FLOAT)
        *t = floattype;
    else if (ds.ts == TS_DOUBLE)
        *t = doubletype;
    else if (ds.ts == TS_LONG + TS_DOUBLE)
        *t = longdoubletype;
    // struct or union

    // make sure dec type is not null
    return dec_type;
}
