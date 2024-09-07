#include "pluto.h"
#include "include/parse.h"

char scs;

// declarations
Node declaration() {
    // declaration_specifiers init_declarator_list[opt]; 
    // static_assert_declaration
    Token tok = lex();
    Node decl;
    if (tok->subtype == _STATIC_ASSERT) {
        restore_tok(&tok);
        decl = static_assert_declaration();
        return decl;
    }
    location l = tok->loc;
    restore_tok(&tok); 
    Type ds = declaration_specifiers();
    // declaration without optional init_declartor_list
    tok = lex();
    if (tok->subtype == SCOL) {
        decl = make_node(ND_TYPE_DECL, PERM);
        decl->loc = l;
        decl->type = ds;
        return decl;
    } else if (tok->subtype == STAR || tok->subtype == LBRAC || tok->type == ID) {
        restore_tok(&tok); 
        decl = init_declarator_list(ds);
        decl->loc = l;
        tok = lex(); 
        if (tok->subtype != SCOL) {
            restore_tok(&tok);
            error(&tok->loc, "expected ';' after declaration");
        }
        return decl;
    }
        
    error(&tok->loc, "expected declarator after declaration specifiers(s)");
    // return dummy decl
    decl = make_node(ND_TYPE_DECL, PERM);
    decl->loc = l;
    decl->type = sinttype;
    return decl;
}


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
    struct dec_spec ds = {.scs = 0, .ts = 0, .tq = 0, .fs = 0, .align_hint = 0};
    int seen = 1;
        while (seen) {
            tok = lex(); // exit loop if FIRST(declarator) or ';'
            if ((tok->type == PUNCT && (tok->subtype == SCOL
                || tok->subtype == LBRAC || tok->subtype == STAR ||
                tok->subtype == COM || tok->subtype == RBRAC ||
                tok->subtype == LSQBRAC)) ||
                (tok->type == ID && !typedef_name(tok)))
                //(tok->type == ID && !lookup(tok->val.strval, types)))
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
        // thread_local and register are ignored
        case TYPEDEF:
            ds->scs |= SCS_TYPEDEF; scs |= SCS_TYPEDEF; return 1;
        case EXTERN:
            ds->scs |= SCS_EXTERN; scs |= SCS_EXTERN; return 1;
        case STATIC:
            ds->scs |= SCS_STATIC; scs |= SCS_STATIC; return 1;
        case _THREAD_LOCAL:
            return 1;
        case AUTO:
            ds->scs |= SCS_AUTO; scs |= SCS_AUTO; return 1;
        case REGISTER:
            return 1;
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

    // typdef_name - consume a previously typedefed type
    if (tok->type == ID) {
        Type typ = typedef_name(tok);
        if (!typ) {
            error(&tok->loc, "type error: identifier doesn't correspond to any valid type");
            typ = sinttype;
            return 1;
        }
        merge_type(typ, ds, t);
        return 1;
    }
    /*
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
    */
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
            if (type_cast()) {
                ds->ts |= TS_ATOMIC;
                return 1;
            }
            return 0;
        case STRUCT: case UNION:
            if (*t)
                error(&tok->loc, "type error: attempting to make more than one struct declaration");
            restore_tok(&tok);
            *t = struct_or_union_specifier();
            return 1;
        case ENUM:
            if (*t)
                error(&tok->loc, "type error: attempting to make more than one enum declaration");
            restore_tok(&tok);
            *t = enum_specifier();
            return 1;
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
        //tag = dtos(genlabel(1));
        if ((tok = lex())->subtype != LCBRAC) {
            restore_tok(&tok);
            error(&tok->loc, "expected struct declaration list in struct/union declaration");
            // error recovery work - return dummy struct/union
            tag = dtos(genlabel(1));
            return make_struct(id, tag, &l);
        }
        restore_tok(&tok);
    } else {
        // tag was defined and struct isn't being defined, 
        // lookup and return previously defined tag
        if ((tok = lex())->subtype != LCBRAC) {
            restore_tok(&tok);
            Symbol sym = lookup(tag, types);
            if (sym && unqual(sym->type)->id != id)
                error(&tok->loc, "two declarations of the same tag should use the same type");
            if (sym)
                return sym->type;
            return make_struct(id, tag, &l);
        }
        restore_tok(&tok);
    }
    su = make_struct(id, tag, &l);
    tok = lex();
    // struct definition
    if (tok->type == PUNCT && tok->subtype == LCBRAC) {
        // struct declaration list
        su->u.sym->u.s.flist = struct_declaration_list(su);
        su->u.sym->defined = 1;
        if ((tok = lex())->type != PUNCT || tok->subtype != RCBRAC) {
            error(&tok->loc, "expected '}' at the end of a struct declaration");
            restore_tok(&tok);
        }
    } else 
        restore_tok(&tok);
    check_struct_decl(su);
    return su;
}

void check_struct_decl(Type su) {
    Field fd = su->u.sym->u.s.flist;
    Type t;
    while (fd) {
        t = fd->type; 
        if ((!iscomplete(t) || isfunc(t)) && (!isarray(t) || fd->next)) {
            error(&loc, "struct/union cannot contain a member with incomplete or function type");
            t = sinttype;
        }
        // todo assign offsets to each field
        fd = fd->next;
    }
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
        if ((tok->type == ID && !typedef_name(tok)) ||
        //if ((tok->type == ID && !lookup(tok->val.strval, types)) ||
            (tok->type == PUNCT && (tok->subtype == COL || 
            tok->subtype == SCOL || tok->subtype == STAR ||
            tok->subtype == LBRAC || tok->subtype == RBRAC ||
            tok->subtype == LSQBRAC)))
            break;

        seen = 0;
        restore_tok(&tok);
        if (seen |= type_specifier(&ds, &t))
            continue;
        if (seen |= type_qualifier(&ds))
            continue;
    }
    if (!(t || ds.ts || ds.tq)) {
        error(&tok->loc, 
        "type error: expected type specifier or type qualifier in struct member declaration");
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
    fd->loc = tok->loc;

    // define bit width if declaration contains bit width
    if (tok->type == PUNCT && tok->subtype == COL) {
        if (!isint(fd->type)) {
            error(&fd->loc, "bit-field members are required to have integer type");
            fd->type = sinttype;
        }
        fd->bitsize = intconstexpr();
        if (fd->bitsize > (unqual(fd->type)->size * 8)|| fd->bitsize < 0) {
            fd->bitsize = 1;
            error(&fd->loc, 
            "bit-field shall be a nonnegative value that doesn't exceed the width of the member");
        }
    } else 
        restore_tok(&tok);
    return fd;
}
Type enum_specifier(){
    // enum identifier[opt] { emunerator_list }
    // enum identifier[opt] { enumerator_list , }
    // enum identifier
    Token tok = lex(); 
    location l = tok->loc;
    if (tok->type != KEYWORD && tok->type != ENUM) {
        restore_tok(&tok);
        error(&tok->loc, "expected enum keyword in enum declaration");
    }
    // extract tag
    char *tag = make_string("", 0);
    tok = lex();
    if (tok->type == ID) {
        tag = tok->val.strval;
        // tag was defined and struct isn't being defined, 
        // lookup and return previously defined tag
        if ((tok = lex())->subtype != LCBRAC) {
            restore_tok(&tok);
            Symbol sym = lookup(tag, types);
            if (!sym) {
                error(&tok->loc, "type specifier 'enum identifier' doesn't correspond to a complete type");
            }
            if (sym && unqual(sym->type)->id != ENUM)
                error(&tok->loc, "two declarations of the same tag should use the same type");
            if (sym)
                return sym->type;
            return make_struct(ENUM, tag, &l);
        }
        restore_tok(&tok);
    } else {
        restore_tok(&tok);
        tag = dtos(genlabel(1));
    }
    Type enm = make_struct(ENUM, tag, &l);
    tok = lex();
    if (tok->type == PUNCT && tok->subtype == LCBRAC) {
        enumerator_list(enm);
        enm->u.sym->defined = 1;
        if ((tok = lex())->type != PUNCT || tok->subtype != RCBRAC) {
            error(&tok->loc, "expected '}' at the end of a struct declaration");
            restore_tok(&tok);
        }
    } else
        restore_tok(&tok);
    return enm;
}
void enumerator_list(Type enm){
    // enumerator
    // enumerator_list , enumerator
    Token tok; 
    int member_val = 0;
    do {
        tok = lex(); 
        if (tok->subtype == RCBRAC) {
            restore_tok(&tok);
            return;
        } else 
            restore_tok(&tok);
        member_val = enumerator(enm, member_val);
    } while ((tok = lex())->subtype == COM);
    restore_tok(&tok);
}
int enumerator(Type enm, int member_val){
    // enumeration_constant
    // enumeration_constant = constant_expression
    Token tok = lex(); 
    char *emem = make_string("", 0);
    // enumeration_constant
    if (tok->type != ID) {
        restore_tok(&tok);
        emem = dtos(genlabel(1));
        error(&tok->loc, "expected an identifier as enum member");
    } else 
        emem = tok->val.strval;

    if ((tok = lex())->subtype != ASSIGN) {
        restore_tok(&tok);
        // assign value and add to table
        Symbol sym = lookup(emem, identifiers); 
        if (sym && sym->scope == level) 
            error(&tok->loc, "attempted to redeclare identifier in enum declaration");
        sym = install(emem, &identifiers, level, PERM);
        sym->type = sinttype;
        sym->u.c.v.i = member_val;
        return ++member_val;
    }
    // constant expression 
    if ((tok = lex())->type != INTCONST) {
        restore_tok(&tok);
        error(&tok->loc, "expected integer constant in rval of enum assignment");
    } else 
        member_val = tok->val.intval.i;
    // assign value and add to table 
    Symbol sym = lookup(emem, identifiers); 
    if (sym)
        error(&tok->loc, "attempted to redeclare identifier in enum declaration");
    sym = install(emem, &identifiers, level, PERM);
    sym->type = sinttype;
    sym->u.c.v.i = member_val;
    return ++member_val;
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
    if (tok->subtype != _ALIGNAS) {
        restore_tok(&tok);
        return 0;
    }
    if ((tok = lex())->subtype != LBRAC) {
        restore_tok(&tok);
        error(&tok->loc, "exptected opening '(' after alignment specifier");
        return 1;
    } 
    // type_name
    tok = lex();
    if (first(ND_TYPENAME, tok)) {
        restore_tok(&tok);
        Type t = type_name();
        if (t) 
            ds->align_hint = unqual(t)->align;
    } else {
        restore_tok(&tok);
        // constant_expression, resolve to int const
        ds->align_hint = intconstexpr();
    }
    if (!ds->align_hint) {
        error(&tok->loc, "unable to determine alignment form alignment specifier");
    }
    if ((tok = lex())->subtype != RBRAC) {
        restore_tok(&tok);
        error(&tok->loc, "expected closing ')' after alignment specifier");
    }
    return 1;
}

Type typedef_name(Token tok) {
    if (tok->type != ID)
        return NULL;
    Symbol sym = lookup(tok->val.strval, identifiers);
    if (!sym || sym->sclass != SCS_TYPEDEF)
        return NULL;
    return sym->type;
}


Node init_declarator_list(Type ds){
    // init_declarator
    // init_declarator_list, init_declarator
    
    // retrieve the first declaration
    Node init_decl = init_declarator(ds);
    // don't check for more if it turns out to be a function definition
    if (init_decl->id == ND_FUNC_DEF)
        return init_decl;
    Token tok = lex();
    // exit early if there is no indication of more declaraions
    if (tok->type != PUNCT || tok->subtype != COM) {
        restore_tok(&tok);
        scs = 0;
        return init_decl;
    }
    restore_tok(&tok);
    
    // group multiple declarations under a list declaration
    Node init_decl_list = make_node(ND_DECL_LIST, PERM);
    add_child(init_decl_list, &init_decl);
    init_decl_list->loc = init_decl->loc;
    while ((tok = lex())->type == PUNCT && tok->subtype == COM) {
        init_decl = init_declarator(ds);
        add_child(init_decl_list, &init_decl);
    }
    restore_tok(&tok);
    // clear off any scs defined in ds and applicable to this list
    scs = 0;
    return init_decl_list;
}

Node init_declarator(Type ds){
    // declarator
    // declarator = initializer

    // retrieve declaration     
    Symbol sym = declarator(ds, 0);
    
    Token tok = lex();
    // a declarator could be followed my a definition (violates the grammar a little)
    if (tok->type == PUNCT && tok->subtype == LCBRAC) {
        // needs updation todo - type mismatch
        restore_tok(&tok);
        return function_definition(sym);
    } else 
        restore_tok(&tok);

    Node dd = make_node(ND_DECL, PERM);
    // first lookup the identifier
    Symbol s = lookup(sym->name, identifiers);
    if (s && s->scope == level) {
        // ensure that types match TODO
        dd->sym = s;
        dd->type = s->type;
        dd->val.strval = s->name;
        dd->loc = s->loc;
    } else {
        dd->sym = install(sym->name, &identifiers, level, level < LOCAL ? PERM : FUNC);
        //dd->sym = install(sym->name, &identifiers, GLOBAL, PERM);
        dd->sym->type = sym->type;
        dd->sym->sclass = sym->sclass;
        dd->sym->loc = sym->loc;
        dd->type = sym->type;
        dd->val.strval = sym->name;
        dd->loc = sym->loc; 
    }

    
    // optional assignment gets added to ND_DECL as a child if present
    if ((tok = lex())->type == PUNCT && tok->subtype == ASSIGN) {
        dd->loc = tok->loc; 
        Node init = initializer();
        s = lookup(dd->sym->name, identifiers);
        if (s && s->scope == level && s->defined)
            error(&dd->loc, "attempted to redefine previously defined identifier");
        dd->sym->defined = 1;
        add_child(dd, &init);
        return dd;
    }
    restore_tok(&tok);
    return dd;
}
Symbol declarator(Type ds, _Bool dad){
    // pointer[opt] direct_declarator
    Token tok = lex();
    // capture optional pointer and update ds
    if (tok->type == PUNCT && tok->subtype == STAR) {
        restore_tok(&tok);
        ds = pointer(ds);
    } else
        restore_tok(&tok);
    // do direct_declarator work
    Symbol sym = direct_declarator(ds, dad);
    if (scs) {
        process_scs(sym);
    }
    return sym;
}

// this function should probably be rewritten - too brittle!
Symbol direct_declarator(Type ds, _Bool dad){
    // identifier
    // (declarator)
    // direct_declarator [ type_qualifier_list[opt] assignment_expression[opt] ]
    // direct_declarator [ static type_qualifier_list[opt] assignment_expression ]
    // direct_declarator [ type_qualifier_list static assignment_expression ]
    // direct_declarator [ type_qualifier_list[opt] * ]
    // direct_declarator ( parameter_type_list )
    // direct_declarator ( identifier_list[opt] ) * K&R style function declarations aren't supported *
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
        //sym->type = ds;
        sym->loc = tok->loc;
        restore_tok(&tok);

    } else {
        sym->name = dtos(genlabel(1)); 
        sym->loc = tok->loc;
        error(&tok->loc, "expected either identifier or '(' in direct declarator");
        restore_tok(&tok);
    }
    
    // retrieve the optional function or array declarator
    tok = lex(); 
    Type decl = ds;
    if (tok->type == PUNCT && (tok->subtype == LBRAC || tok->subtype == LSQBRAC)) {
        restore_tok(&tok);
        decl = func_or_array_decl(ds);
    } else 
        restore_tok(&tok);
    tok = lex();
    // if this declaration was a nested declaration, don't try to update the type just yet
    if (tok->type == PUNCT && tok->subtype == RBRAC) {
        restore_tok(&tok);
        if (sym->type && (isptr(sym->type) || 
                isfunc(sym->type) || isarray(sym->type))) {
            Type t = sym->type;
            while (t->type && t->type != nulltype) {
                if (is_basetype(t->type)) {
                    return sym;
                }
                t = t->type;
            }
            t->type = decl;
        } else 
            sym->type = decl;
        return sym;
    }
    // not a function or array declaration
    if (tok->type == PUNCT && (tok->subtype == COM || tok->subtype == SCOL 
                || tok->subtype == COL || tok->subtype == ASSIGN
                || tok->subtype == LCBRAC)) {
        restore_tok(&tok);
        if (sym->type && (isptr(sym->type) 
            || isfunc(sym->type) || isarray(sym->type))) {
            Type t = sym->type;
            while (t->type && t->type != nulltype) {
                // type was known, no updation needed
                if (is_basetype(t->type)) {
                    return sym;
                }
                t = t->type;
            }                
            t->type = decl;

        } else 
            sym->type = decl;
    } else {
        // else error?
        error(&tok->loc, "internal error, fix!!");
        restore_tok(&tok);
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
    if (!ds)
        ds = nulltype;
    Token tok = lex(); 
    if (tok->type == PUNCT && tok->subtype == LSQBRAC) {
        // get array declaration
        restore_tok(&tok);
        Type arr = array_decl(ds);
        // recursively get subtype
        Type arr_child = func_or_array_decl(ds);
        // update array with more precise type and size
        arr->type = arr_child;
        /*
        if (isqual(arr_child) && arr_child->type) 
            arr->size = arr->size * (arr_child->type->size);
        else 
            arr->size = arr->size * arr_child->size;
        */
        arr->size = arr->size;
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
    // optional type_qualifier_list and STATIC
    _Bool stat = 0;
    int size = 0;
    if (tok->subtype == STATIC) {
        stat = 1;
    } else 
        restore_tok(&tok);
    Type tq = type_qualifier_list(NULL);
    if ((tok = lex())->subtype == STATIC)
        stat = 1;
        // error if scope not param
    else 
        restore_tok(&tok);
    // variable length array
    if ((tok = lex())->subtype == STAR)
        size = 0;
        // error if scope not param
    else 
        restore_tok(&tok);
    // size not defined
    if ((tok = lex())->subtype == RSQBRAC)
        return make_array(ds, size, 0);
    else 
        restore_tok(&tok);
        
    // assignment expression
    Node assign_expr = assignment_expression();
    if (assign_expr && !isint(assign_expr->type)) 
        error(&assign_expr->loc, "expression denoting array size should have integer type");
    arr = make_array(ds, size, 0);
    // determine is assign_expr needs to be evaluated
    _Bool flag = 0;
    size = solve_intconst(assign_expr, &flag);
    if (flag) 
        arr->arr_size = assign_expr;
    else
        arr->size = size;

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
    if (first(ND_ABS_DECL, tok)) {
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

Node identifier_list(){
    // identifier
    // identifier , identifier
}
Type type_name(){
    // specifier_qualifier_list abstract_declarator[opt]
    return declarator(specifier_qualifier_list(), 1)->type;
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
        Node init_list = initializer_list();
        if ((tok = lex())->subtype != RCBRAC) {
            restore_tok(&tok);
            error(&tok->loc, "expected closing '}' after declaration initializer list");
        }
        return init_list;
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
    } else 
        restore_tok(&tok);
    Node init_child = initializer();
    add_child(init, &init_child);
    return init;
}
Node designation(){
    // designator_list =
    Node desig_list = designator_list();
    Token tok = lex();
    if (tok->subtype != ASSIGN) {
        error(&tok->loc, "expected '=' sign after designator list in initializer");
    }
    return desig_list;
}
Node designator_list(){
    // designator
    // designator_list designator
    Node desig_list = make_node(ND_DESIG_LIST, PERM);
    Node desig = designator();
    desig_list->loc = desig->loc;
    add_child(desig_list, &desig);
    Token tok = lex();
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
        if (tok->subtype != RSQBRAC) {
            error(&tok->loc, "expected closing ']' in  designator");
        }
        return sel;
    }
    if (tok->subtype != DOT) {
        restore_tok(&tok);
        error(&tok->loc, "expected designator in designator list");
    }
    Node dot = make_node(ND_DOT, PERM);
    dot->loc = tok->loc;
    Node id = make_node(ND_ID, PERM);
    tok = lex();
    if (tok->type != ID) {
        error(&tok->loc, "expected identifier after '.' in designator");
        id->val.strval = dtos(genlabel(1));
        id->loc = tok->loc;
        add_child(dot, &id);
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
void process_scs(Symbol sym) {
    if (!sym || !scs)
        return;
    switch(scs) {
        case SCS_TYPEDEF:   
            sym->sclass |= SCS_TYPEDEF;     break;
        case SCS_EXTERN:    
            sym->sclass |= SCS_EXTERN;      break;
        case SCS_STATIC:    
            sym->sclass |= SCS_STATIC;      break;
        case SCS_AUTO:      
            sym->sclass |= SCS_AUTO;        break; 
        case SCS_REGISTER:
            sym->sclass |= SCS_REGISTER;    break;
        case SCS_THREAD_LOCAL:
            sym->sclass |= SCS_THREAD_LOCAL;break;
        case SCS_TYPEDEF + SCS_EXTERN:
            sym->sclass |= SCS_TYPEDEF;
            sym->sclass |= SCS_EXTERN;      break;
        case SCS_TYPEDEF + SCS_STATIC:
            sym->sclass |= SCS_TYPEDEF; 
            sym->sclass |= SCS_STATIC;      break;
        case SCS_TYPEDEF + SCS_AUTO:
            sym->sclass |= SCS_TYPEDEF; 
            sym->sclass |= SCS_AUTO;        break;
        case SCS_TYPEDEF + SCS_REGISTER:
            sym->sclass |= SCS_TYPEDEF; 
            sym->sclass |= SCS_REGISTER;    break;
        case SCS_TYPEDEF + SCS_THREAD_LOCAL:
            sym->sclass |= SCS_TYPEDEF; 
            sym->sclass |= SCS_THREAD_LOCAL;break;

        default:
            error(&loc, "attempting to declare declarator with more than one storage class specifier");
            break;
    }
}

void merge_type(Type typ, struct dec_spec *ds, Type *t) {
    // add type qualifier
    if (isqual(typ)) {
        switch(typ->id) {
            case _ATOMIC:           
                ds->tq |= TQ_ATOMIC;                        
                break;
            case CONST:             
                ds->tq |= TQ_CONST;                         
                break;   
            case VOLATILE:          
                ds->tq |= TQ_VOLATILE;                      
                break;
            case RESTRICT:          
                ds->tq |= TQ_RESTRICT;                      
                break;
            case CONST+VOLATILE:    
                ds->tq |= TQ_CONST; ds->tq |= TQ_VOLATILE;  
                break;
            case CONST+RESTRICT:    
                ds->tq |= TQ_CONST; ds->tq |= TQ_RESTRICT;  
                break;
            case VOLATILE+RESTRICT: 
                ds->tq |= TQ_VOLATILE; ds->tq |= TQ_RESTRICT; 
                break;
            case CONST+VOLATILE+RESTRICT:
                ds->tq |= TQ_CONST; ds->tq |= TQ_VOLATILE; ds->tq |= TQ_RESTRICT;
                break;
        }
        typ = unqual(typ);
    }
    *t = typ;
}
Type build_type(struct dec_spec ds, Type ty) {
    // build type
    Type dec_type = NULL;
    // add type qualifiers, if specified
    if (ds.tq & TQ_ATOMIC)
       dec_type = make_type(_ATOMIC, NULL, 0, 0, NULL, make_string("_Atomic", 7));
    else if (ds.tq == TQ_CONST)
        dec_type = make_type(CONST, NULL, 0, 0, NULL, make_string("const", 5));
    else if (ds.tq == TQ_VOLATILE)
        dec_type = make_type(VOLATILE, NULL, 0, 0, NULL, make_string("volatile", 8));
    else if (ds.tq == (TQ_CONST + TQ_VOLATILE))
        dec_type = make_type(CONST + VOLATILE, NULL, 0, 0, NULL, make_string("const volatile", 14));
    else if (ds.tq == (TQ_CONST + TQ_RESTRICT))
        dec_type = make_type(CONST+RESTRICT, NULL, 0, 0, NULL, make_string("const restrict", 14)); 
    else if (ds.tq == (TQ_VOLATILE + TQ_RESTRICT))
        dec_type = make_type(VOLATILE+RESTRICT, NULL, 0, 0, NULL, make_string("volatile restrict", 17));
    else if (ds.tq == (TQ_CONST + TQ_VOLATILE + TQ_RESTRICT))
        dec_type = make_type(CONST+VOLATILE+RESTRICT, NULL, 0, 0, NULL, make_string("const volatile restrict", 23));
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
    // make sure dec type is not null
    return dec_type;
}


// this version of handle su is incorrect as it doesn't handle 
// members that are arrays structs/unions and function ptrs - shouldn't be used
void handle_su(Type *su) {
    // if the struct/union is qualified the qualifiers applies to each member
    if (isqual(*su)) {
        int qualval = (*su)->id;
        *su = unqual(*su);
        Field fls = (*su)->u.sym->u.s.flist;
        while (fls) {
            Type flst = fls->type;
            if (!isqual(flst))
                fls->type = qual(qualval, flst);
            // no action needed if both specify the same qualifier
            else if (isqual(flst) && qualval != flst->id) {
                int *flstqval = &(flst->id);
               switch (qualval) {
                    case _ATOMIC: 
                        fls->type = qual(_ATOMIC, flst->type); break;
                    case CONST:
                        if (*flstqval == VOLATILE) 
                            *flstqval += CONST;
                        break;
                        // if its const+volatile or atomic, then no action is needed
                    case VOLATILE:
                        if (*flstqval == CONST)
                            *flstqval += VOLATILE;
                        break;
                        // if its const+volatile or atomic, then no action is needed
                    case CONST+VOLATILE:
                        if (*flstqval != _ATOMIC)
                            *flstqval = CONST+VOLATILE;
                        break;
               }

            }
            fls = fls->next;
        }
    }

}



