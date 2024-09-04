#include "pluto.h"
#include "include/parse.h"

// type operators - CHAR, SHORT, INT, LONG, FLOAT, DOUBLE, UNSIGNED, _BOOL, _COMPLEX, 
// VOID, STRUCT, ENUM, UNION, ARRAY, POINTER, FUNCTION, CONST, RESTRICT, VOLATILE, _ATOMIC

// citation


// pre-installed types 
Type booltype;      // _Bool
Type sbooltype;     // signed _Bool
Type ubooltype;     // unsigned _Bool

Type chartype;      // char 
Type schartype;     // signed char
Type uchartype;     // unsigned char 

Type shorttype;     // signed short 
Type sshorttype;     // signed short 
Type ushorttype;    // unsigned short  

Type inttype;       // signed short
Type sinttype;      // signed int
Type uinttype;      // unsigned int

Type longtype;      // signed long
Type slongtype;     // signed long
Type ulongtype;     // unsigned long

Type longlongtype;  // signed long long
Type slonglongtype; // signed long long
Type ulonglongtype; // unsigned long long  

Type floattype;     // float
Type doubletype;    // double
Type longdoubletype;// long double

Type voidtype;      // void 
Type nulltype;      // null
Type defaulttype;   // default

Type voidptype;     // void *
Type charptype;     // char *

Type uptype;        // unsigned pointer 
Type sptype;        // signed pointer

// maintain only a single copy of each type
#define TYPE_TAB_SIZE 128
static struct typeentry {
    struct type type; 
    struct typeentry *next; 
} *typetable[TYPE_TAB_SIZE];

// highest level at which a type was defined. 
// this value ranges from GLOBAL to LOCAL+k. 
// where k is either equal to orusualy lagging 
// behind the current highest local scope
static int maxlevel;

// used while creating new pointer types
static Symbol pointersym;

Type make_type(int id, Type t, int size, int align, void *sym, char *strrep) {
    
    // first scan the typetable to see if the type already exits
    unsigned int h = (id ^ ((char)(intptr_t)t)) & (TYPE_TAB_SIZE - 1); 
    struct typeentry *tn;
    if (id != FUNCTION && (id != ARRAY) && id!= POINTER) 
        for (tn = typetable[h]; tn; tn = tn->next)
            if (tn->type.id == id && tn->type.type == t && tn->type.u.sym == sym
                    && tn->type.size == size && tn->type.align == align) 
                return &tn->type;
    
    // type doesn't exist, create a new entry in the type table
    struct typeentry *e = alloc(sizeof(struct entry), PERM); 
    e->type.id = id; 
    e->type.type = t;
    e->type.size = size; 
    e->type.align = align;
    e->type.u.sym = sym;
    e->type.strrep = strrep;
    e->next = typetable[h];
    typetable[h] = e; 
    return &e->type;
}


void dumptypes() {
    struct typeentry *tn;
    for (int h = 0; h < TYPE_TAB_SIZE; h++) {
        if (typetable[h]) 
            fprintf(stdout, "Bucket [%d]\n", h);
        
        for (tn = typetable[h]; tn; tn = tn->next)
            fprintf(stdout, "> %s\n", ttos(&tn->type));
    }

}


Type make_ptr(Type t) {
    return make_type(POINTER, t, 8, 8, pointersym, make_string("*", 1));
}


Type deref(Type t) {
    if (isptr(t))
        t = t->type;
    else 
        error(&loc, "type error: attempted to dereference non-pointer type");
    return isenum(t) ? unqual(t)->type : t;
}

Type make_array(Type t, int size, int align) {
    if (isfunc(t)) {
        error(&loc, "type error: attempted to create an array with function type");
        return make_array(inttype, size, 0);
    }
    if (level > GLOBAL && isarray(t) && t->size == 0) 
        error(&loc, "missing array size");
    if (t->size == 0) {
        if (unqual(t) == voidtype)
            error(&loc, "illegal to declare an array of type void");
    } else if (size > INT_MAX / t->size) {
        error(&loc, "size of array exceeds INT_MAX");
        size = 1;
    }
    // size will be added correctly later
    return make_type(ARRAY, t, size, align ? align : t->align, NULL, make_string("array", 5));
}

Type atop(Type t) {
    if (isarray(t))
        return make_ptr(t->type);
    error(&loc, "type error: expected array type");
    return make_ptr(t);
}

Type array_type(Type t) {
    if (isptr(t))
        return deref(t);
    return deref(atop(t));
}

Type qual(int id, Type t) {
    // check type qualifier rules: TODO 
    if (id & TQ_ATOMIC) {
        return make_type(_ATOMIC, t, 0, 0, NULL, 
                make_string("_Atomic", 7));
    }
    switch(id) {
        case CONST: 
            id = TQ_CONST; break;
        case VOLATILE: 
            id = TQ_VOLATILE; break;
        case RESTRICT:
            id = TQ_RESTRICT; break;
        case CONST+VOLATILE:
            id = TQ_CONST+TQ_VOLATILE; break;
        case CONST+RESTRICT:
            id = TQ_CONST+TQ_RESTRICT; break;
        case CONST+VOLATILE+RESTRICT:
            id = TQ_CONST+TQ_VOLATILE+TQ_RESTRICT; break;
    }
    switch (id) {
        case TQ_CONST: 
            return make_type(CONST, t, 0, 0, NULL, 
                    make_string("const", 5));
        case TQ_RESTRICT:
            return make_type(RESTRICT, t, 0, 0, NULL, 
                    make_string("restrict", 8));
        case TQ_VOLATILE:
            return make_type(VOLATILE, t, 0, 0, NULL, 
                    make_string("volatile", 8));
        case TQ_CONST + TQ_VOLATILE:
            return make_type(CONST+VOLATILE, t, 0, 0, NULL, 
                    make_string("const volatile", 14));
        case TQ_CONST + TQ_RESTRICT:
            return make_type(CONST+RESTRICT, t, 0, 0, NULL, 
                    make_string("const restrict", 14));
        case TQ_CONST + TQ_RESTRICT + TQ_VOLATILE:
            return make_type(CONST+VOLATILE+RESTRICT, t, 0, 0, NULL, 
                    make_string("const volatile restrict", 23));
    }
    // don't qual if it didn't match any case
    return t;
}

int isqual(Type t) {
    if (!t)
        return 0;
    switch(t->id) {
        case CONST: case RESTRICT: case VOLATILE: case _ATOMIC:
        case CONST + VOLATILE: case CONST + RESTRICT: 
        case VOLATILE + RESTRICT: 
        case CONST + VOLATILE + RESTRICT:
            return 1;
        default:
            return 0;
    }
    return 0;
}

Type make_func(Type t, Vector proto) {
    if (t && (isarray(t) || isfunc(t)))
        error(&loc, "illegal return type"); 
    t = make_type(FUNCTION, t, 0, 0, NULL, make_string("function", 8));
    t->u.f.proto = proto;
    return t;
}

Type freturn(Type t) {
    if (isfunc(t))
        return t->type;
    error(&loc, "type error: attempted to access return value of non-function type");
    return inttype;
}

int variadic(Type t) {
    if (isfunc(t) && t->u.f.proto) {
        int i = t->u.f.proto->size - 2;
        Symbol last = vec_get(t->u.f.proto, i);
        return i >= 1 && last && last->type == voidtype;
    }
    return 0;
}

// TODO: struct and enum functions
Type make_struct(int id, char *tag, Location l) {
    Symbol p;
    if (*tag == 0)
        tag = dtos(genlabel(1));
    else 
        if ((p = lookup(tag, types)) != NULL && (p->scope == level
                    || p->scope == PARAM && level == PARAM + 1)) {
            if (p->type->id == id && !p->defined)
                return p->type;
            error(&loc, "redefinition of previously defined struct/union");
        }
    p = install(tag, &types, level, PERM);
    if (id == STRUCT) 
        p->type = make_type(STRUCT, NULL, 0, 0, p, make_string("struct", 6));
    else if (id == UNION)
        p->type = make_type(UNION, NULL, 0, 0, p, make_string("union", 5));
    else 
        p->type = make_type(ENUM, NULL, sinttype->size, 
                sinttype->align, p, make_string("enum", 4));
    if (p->scope > maxlevel)
        maxlevel = p->scope;
    p->loc = *l;
    return p->type; 
}

Field make_field(char *name, Type t, Type ft) {
    Field p, *q = &t->u.sym->u.s.flist;

    if (!*name)
        name = dtos(genlabel(1));
    for (p = *q; p; q = &p->next, p = *q)
        if (p->name == name)
            error(&loc, "attempted to create struct with duplicate field");
    p = alloc(sizeof(field), PERM);
    *q = p; 
    p->name = name;
    p->type = ft;
    return p;
}



Type xxinit(int id, Type t, char *strrep, char *name, int size, int align) {
    Symbol sym = install(make_string(name, strlen(name)), &types, GLOBAL, PERM);
    Type ty = make_type(id, t, size, align, sym, make_string(strrep, strlen(strrep))); 
    sym->type = ty;
    switch(ty->id) {
        case _BOOL:
            sym->u.limits.max.b = 1;
            sym->u.limits.min.b = 0;
            break;
        case CHAR: 
            sym->u.limits.max.c = CHAR_MAX;
            sym->u.limits.min.c = CHAR_MIN;
            break;
        case SHORT:
            sym->u.limits.max.s = SHRT_MAX;
            sym->u.limits.min.s = SHRT_MIN;
            break;
        case INT: 
            // signed int and signed long 
            if (ty->size == 4) {
                sym->u.limits.max.i = INT_MAX;
                sym->u.limits.min.i = INT_MIN;
            }
            // signed long long
            if (ty->size == 8) {
                sym->u.limits.max.ll = LLONG_MAX; 
                sym->u.limits.min.ll = LLONG_MIN;
            }
            break;
        case UNSIGNED:
            switch(ty->type->id) {
                case CHAR:
                    sym->u.limits.max.uc = UCHAR_MAX;
                    sym->u.limits.min.uc = 0;
                    break;
                case SHORT:
                    sym->u.limits.max.us = USHRT_MAX;
                    sym->u.limits.min.us = 0;
                    break;
                case INT: 
                    sym->u.limits.max.ui = UINT_MAX; 
                    sym->u.limits.min.ui = 0;
                    break;
                case LONG: 
                    sym->u.limits.max.ul = ULONG_MAX;
                    sym->u.limits.min.ul = 0; 
                    break;
                case SLLONG:
                    sym->u.limits.max.ull = ULLONG_MAX;
                    sym->u.limits.min.ull = 0;
                    break;
            }
            break;
        case SIGNED:
            switch(ty->type->id) {
                case CHAR:
                    sym->u.limits.max.uc = SCHAR_MAX;
                    sym->u.limits.min.uc = SCHAR_MIN;
                    break;
                case SHORT:
                    sym->u.limits.max.us = SHRT_MAX;
                    sym->u.limits.min.us = SHRT_MIN;
                    break;
                case INT: 
                    sym->u.limits.max.ui = INT_MAX; 
                    sym->u.limits.min.ui = INT_MIN;
                    break;
                case LONG: 
                    sym->u.limits.max.ul = LONG_MAX;
                    sym->u.limits.min.ul = LONG_MIN; 
                    break;
                case SLLONG:
                    sym->u.limits.max.ull = LLONG_MAX;
                    sym->u.limits.min.ull = LLONG_MIN;
                    break;

            }
            break;
        case FLOAT:
            if (ty->size == 4) 
                sym->u.limits.max.f = FLT_MAX;
            if (ty->size == 8)
                sym->u.limits.max.d = DBL_MAX;
            if (ty->size == 16)
                sym->u.limits.max.ld = LDBL_MAX;
            break;
    }
    return ty;
}

void typeinit() {
    static int inited; 
    if (inited)
        return;
    inited = 1;
    booltype = xxinit(_BOOL, NULL, "_Bool", "_Bool", 1, 1);
    sbooltype = booltype;
    ubooltype = booltype;

    chartype = xxinit(CHAR, NULL, "char", "char", 1, 1); 
    schartype = xxinit(SIGNED, chartype, "signed", "signed char", 1, 1);
    uchartype = xxinit(UNSIGNED, chartype, "unsigned", "unsigned char", 1, 1); 

    shorttype = xxinit(SHORT, NULL, "short", "short", 2, 2);
    sshorttype = xxinit(SIGNED, shorttype, "signed", "signed short", 2, 2);
    ushorttype = xxinit(UNSIGNED, shorttype, "unsigned", "unsigned short", 2, 2);
    
    inttype = xxinit(INT, NULL, "int", "int", 4, 4);
    sinttype = xxinit(SIGNED, inttype, "signed", "signed int", 4, 4);
    uinttype = xxinit(UNSIGNED, inttype, "unsigned", "unsigned int", 4, 4);
    
    longtype = xxinit(LONG, NULL, "long int", "long int", 4, 4); 
    slongtype = xxinit(SIGNED, longtype, "signed", "siged long int", 4, 4);
    ulongtype = xxinit(UNSIGNED, longtype, "unsigned", "unsigned long int", 4, 4);

    longlongtype = xxinit(SLLONG, NULL, "long long int",  "long long int", 8, 8); 
    slonglongtype = xxinit(SIGNED, longlongtype, "signed", "signed long long int", 8, 8);
    ulonglongtype = xxinit(UNSIGNED, longlongtype, "unsigned", "unsigned long long int", 8, 8);

    floattype = xxinit(FLOAT, NULL, "float", "float", 4, 4); 
    doubletype = xxinit(SDOUBLE, NULL, "double", "double", 8, 8);
    longdoubletype = xxinit(LDOUBLE, NULL, "long double", "long double", 16, 8);

    voidtype = xxinit(VOID, NULL, "void", "void", 0, 0);
    nulltype = xxinit(NULLT, NULL, "null", "null", 1, 1);
    defaulttype = xxinit(DEFAULT, NULL, "default", "default", 0, 0);

    pointersym = install(make_string("*", 1), &types, GLOBAL, PERM); 
    pointersym->u.limits.max.p = (void *) ULLONG_MAX;
    pointersym->u.limits.min.p = 0;

    voidptype = make_ptr(voidtype);
    charptype = make_ptr(chartype);
    uptype = ulonglongtype;
    sptype = longlongtype;

}

void rmtypes(int lev) {
    // don't need to do anything if maxlevel is lower than lev
    if (maxlevel >= lev) {
        int i; 
        maxlevel = 0;
        for (int i = 0; i < 128; i++) {
            // remove types with u.sym->scope >= lev
            struct typeentry *tn, **tb = &typetable[i];
            while ((tn = *tb) != NULL) {
                if (tn->type.id == FUNCTION) {
                    tb = &tn->next;
                } else if (tn->type.u.sym && tn->type.u.sym->scope >= lev) {
                    *tb = tn->next;
                } else {
                    if (tn->type.u.sym && tn->type.u.sym->scope > maxlevel) 
                        maxlevel = tn->type.u.sym->scope;
                    tb = &tn->next;
                }
            }
        }
    }
}

_Bool is_basetype(Type t) {
    t = unqual(t);
    return isarray(t) || isstructunion(t) || isarith(t) || (t->id == VOID);
}



char *array_to_string(Type t) {
    char *str = alloc(200, PERM);
    *str = '\0';
    Type tcpy = t;
    if (isqual(tcpy)) {
        strcat(str, t->strrep);
        strcat(str, " ");
        t = t->type; 
    }
    while (isarray(t)) {
        t = t->type;
    }
    // print base type
    strcat(str, ttos(t));
    strcat(str, " ");
    
    // array braces with size enclosed if available
    while (isarray(tcpy)) {
        strcat(str, "[");
        if (tcpy->size)
            strcat(str, dtos(tcpy->size));
        strcat(str, "]");
        tcpy = tcpy->type;
    }
    return str;
}

char *ttos(Type t) {
    char *str = alloc(200, PERM);
    *str = '\0';
    _Bool ls = 0;
    while (t) {
        if (isarray(t)) {
            return array_to_string(t);
        } else if (isstructunion(t)) {
             strcat(str, struct_to_string(t));
             if (isqual(t)) {
                t = t->type;
             }
        } else if (isfunc(t)) {
            strcat(str, func_to_string(t));
            return str; 
        } else if (isptr(t) && isqual(t)) {
            strcat(str, ttos(t->type));
            strcat(str, " ");
            strcat(str, t->strrep);
            return str;
        } else if (isptr(t)) {
            int cnt = 1;
            while (t->type && t->type->id == POINTER) {
                cnt++; 
                t = t->type;
            }
            strcat(str, ttos(t->type));
            strcat(str, " ");
            for (int i = 0; i < cnt; i++) 
                strcat(str, "*");
            return str;
        } else {
            if (ls)
                strcat(str, " ");
            else 
                ls = 1;
            strcat(str, t->strrep);
        }
        t = t->type;
    }
    return str;
}

char *struct_to_string(Type t) {
    char *str = alloc(200, PERM);
    *str = '\0';
    // print type specifier, type qualifier and keyword
    if (isqual(t)) {
        strcat(str, t->strrep);  
        strcat(str, " ");
        t = t->type;    
    }
    strcat(str, t->strrep);
    strcat(str, " ");
    strcat(str, t->u.sym->name);

    // print prototype

    Field fd = t->u.sym->u.s.flist;
    _Bool print_body = 0;
    _Bool ls = 0;
    if (fd)
        print_body = 1;
    if (print_body)
        strcat(str, "{");
    while (fd) {
        if (!ls) 
            ls = 1;
        else
            strcat(str, " ");
        strcat(str, ttos(fd->type));
        strcat(str, " ");
        strcat(str, fd->name);
        if (fd->bitsize) {
            strcat(str, ":");
            strcat(str, dtos(fd->bitsize));
        }
        
        strcat(str, ";");
        fd = fd->next;
    }
    if (print_body)
        strcat(str, "}");
    return str;
}

char *func_to_string(Type t) {
    char *str = alloc(200, PERM);
    *str = '\0';
    // print return type
    strcat(str, ttos(t->type));
    
    // print function prototoype
    strcat(str, " (");
    Vector proto = t->u.f.proto;
    // print first param without space and comma
    Symbol param = (Symbol) vec_get(proto, 0);
    strcat(str, ttos(param->type));
    strcat(str, " ");
    strcat(str, param->name);
    int size = variadic(t) ? proto->size - 2 : proto->size - 1;
    for (int i = 1; i < size; i++) {
        strcat(str, ", ");
        param = (Symbol) vec_get(proto, i);
        strcat(str, ttos(param->type));
        strcat(str, " ");
        strcat(str, param->name);
    }
    if (variadic(t))
        strcat(str, ", ...");
    strcat(str, ")");
    return str;
}


// type checking functions 
_Bool match_proto(Node arg_list, Type t) {
    int call_params = arg_list->num_kids;
    _Bool isvariadic = variadic(t);
    int proto_params = t->u.f.proto->size;
    proto_params -= isvariadic ? 2 : 1;
    Node arg = arg_list->kids;
    Symbol p = (Symbol) vec_get(t->u.f.proto, 0);

    // if the t is variadic then proto_params should be <= call_params
    for (int i = 0; i < proto_params; i++) {
        p = (Symbol) vec_get(t->u.f.proto, i);
        if (!p) {
            error(&loc, "internal erorr while matching function prototype");
            return 0;
        }
        if (!arg) {
            error(&loc, "attempting to make function call with insufficient arguments");
            return 0;
        }
        // incorrect update once type compatibality is done
        if (arg->type != p->type) {
            error(&loc, "given arguement is incompatile with function declaration");
            return 0;
        }
        arg = arg->nxt;
    }
    // if not variadic then number of arguments should match
    if (!isvariadic && arg) {
        error(&loc, "attempting to make function call with extra parameters");
        return 0;
    }
    return 1;
}
_Bool expect_noargs(Type t) {
    int num_args = t->u.f.proto->size;
    if (num_args == 2) {
        Symbol p1 = (Symbol) vec_get(t->u.f.proto, 0);
        Symbol p2 = (Symbol) vec_get(t->u.f.proto, 1);
        if (p1 && !p2 && p1->type == voidtype)
            return 1;
    }
    error(&loc, "attempting to make function call with insufficient arguments");
    return 0;
}


Field get_member(Type su, char *member) {
    if (isqual(su)) {
        su = unqual(su);
    }
    Field fls = su->u.sym->u.s.flist;
    while (fls) {
        if (fls->name == member) {
            return fls;
        }
        fls = fls->next;
    }
    return NULL;
}
/*
Field update_qual(int qualval, Field *fls) {
    // if qualval wasn't specified or, 
    // if qualval matches that of fls don't take any action
    if (!qualval)
        return *fls;
    Type flst = (*fls)->type;
    if (isqual(flst) && flst->id == qualval)
        return *fls;
    // fls wasn't qualified, update it with the qualified version 
    if (!isqual(flst)) {
        (*fls)->type = qual(qualval, (*fls)->type);
        return *fls;
    }
    // at this point fls is qualified and doesn't match qualval
    int *flstqval = &(flst->id);
     switch (qualval) {
        case _ATOMIC: 
            (*fls)->type = qual(TQ_ATOMIC, flst->type); break;
            // if qualval is atomic, it doesn't matter what fls's previous qualval was
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
     return *fls;
}
*/
Type update_qual(int qualval, Type t) {
    // if qualval wasn't specified or, 
    // if qualval matches that of fls don't take any action
    if (!qualval)
        return t;
    if (isqual(t) && t->id == qualval)
        return t;
    if (!isqual(t))
        return qual(qualval, t);
    // at his point t is qualified and doesn't match qualval
    int newqual = qualval;
    switch(qualval) {
        case CONST:
            if (t->id == VOLATILE)
                newqual += VOLATILE;
            break;
        case VOLATILE:
            if (t->id == CONST)
                newqual += CONST;
            break;
        case CONST + VOLATILE:
            if (t->id == _ATOMIC)
                newqual = _ATOMIC;
            break;
    }
    // new qualifier gets applied to the base type
    return qual(newqual, t->type);
}

Type fieldtype(Type su, Type f) {
    int qualval = 0;
    if (isqual(su))
        qualval = su->id;
    if (qualval)
        return update_qual(qualval, f);
    return f;
}

Type base_type(Type t) {
    if (!t)
        return t;
    t = unqual(t);
    if (!t)
        return t;
    if (isptr(t) || isarray(t))
        return base_type(t->type);
    return t;
}

_Bool is_lval(Symbol s) {
    // temporary variables aren't lvals
    if (!s || s->temporary)
        return 0;
    Type bt = base_type(s->type);
    if (bt == voidtype)
        return 0;
    // more possible cases

    return 1;
}

_Bool modifiable_lval(Type t) {
    // arrays and const qualified types aren't m'lvals
    if (!t || isarray(t) || isconst(t))
        return 0;
    // struct/unions are not modifyable if any member is
    // const qualified
    if (!isstructunion(t))
        return 1;
    Field f = t->u.sym->u.s.flist;
    while (f) {
        if (!modifiable_lval(f->type))
            return 0;
        f = f->next;
    }
    return 1;
}

Type promote(Type t) {
    Type tt = unqual(t);
    // integer promotion only applies to types that
    // have a rank lower than (signed) int
    if (tt == booltype || tt == schartype ||
        tt == uchartype || tt == shorttype ||
        tt == sshorttype || tt == ushorttype ||
        tt == inttype || tt == sinttype)
        return sinttype;
    return unqual(t);
}

_Bool iscomplete(Type t) {
    if (t == voidtype)
        return 0;
    if (isarray(t))
        if (!unqual(t)->size)
            return 0;
    if (isstructunion(t)) {
        if (!t->u.sym->defined)
            return 0;
    } 
    return 1;
}

size_t struct_size(Type t) {
    if (!isstructunion(t))
        error(&loc, "struct size expected type of sturct or union");
    // todo - calculate 
    return 5;
}

char get_rank(Type t) {
    // (signed) long long int > (signed) long int > 
    // (signed) int > (signed) short > signed char  > _Bool
    // signed and unsigned counter parts have equal ranks
    if (t == slonglongtype || t == ulonglongtype || t == longlongtype)
        return 6;
    if (t == slongtype || t == ulongtype || t == longtype)
        return 5;
    if (t == sinttype || t == uinttype || t == inttype)
        return 4;
    if (t == sshorttype || t == ushorttype || t == shorttype)
        return 3;
    if (t == schartype || t == uchartype || t == chartype)
        return 2;
    if (t == sbooltype || t == ubooltype || t == sbooltype)
        return 1;
    else 
        error(&loc, "passed invalid type to get rank");
    return 0;
}

Type get_unsigned(Type t) {
    char rt = get_rank(t); 
    switch(rt) {
        case 6: return ulonglongtype;
        case 5: return ulongtype;
        case 4: return uinttype;
        case 3: return ushorttype;
        case 2: return uchartype;
        case 1: return ubooltype;
    }
    return uinttype;
}

unsigned long long int get_integer_max(Type t) {
#define COND(a, b) if (t == a) return b
    if (t == booltype || t == sbooltype || t == ubooltype)
        return 1;
    COND(chartype, CHAR_MAX);
    COND(uchartype, UCHAR_MAX);
    COND(schartype, SCHAR_MAX);
    
    COND(shorttype, SHRT_MAX);
    COND(ushorttype, USHRT_MAX);
    COND(sshorttype, SHRT_MAX); 

    COND(inttype, INT_MAX); 
    COND(uinttype, UINT_MAX);
    COND(sinttype, INT_MAX);

    COND(longtype, LONG_MAX);
    COND(ulongtype, ULONG_MAX);
    COND(slongtype, LONG_MAX); 

    COND(longlongtype, LLONG_MAX); 
    COND(ulonglongtype, ULLONG_MAX);
    COND(slonglongtype, LLONG_MAX);
#undef COND
    return 0;
}


Type usual_arithmetic_conversion(Type t1, Type t2) {
    // qualifiers don't impact uac
    t1 = unqual(t1); t2 = unqual(t2);
    if (t1 == longdoubletype || t2 == longdoubletype)
        return longdoubletype;
    if (t1 == doubletype || t2 == doubletype)
        return doubletype;
    if (t1 == floattype || t2 == floattype)
        return floattype;
    t1 = promote(t1); t2 = promote(t2);
    if (t1 == t2)
        return t1;
    if ((isunsigned(t1) && isunsigned(t2)) ||
         (issigned(t1) && issigned(t2))) {
        if (get_rank(t1) > get_rank(t2))
            return t1;
        return t2;
    }
    if (isunsigned(t1) && issigned(t2)) {
        if (get_rank(t1) >= get_rank(t2))
            return t1;
        else if (get_integer_max(t2) >= get_integer_max(t1))
            return t2;
        return get_unsigned(t2);
    // implied isunsigned(t2) && issigned(t1)
    } else {
        if (get_rank(t2) >= get_rank(t1))
            return t2; 
        else if (get_integer_max(t1) >= get_integer_max(t2))
            return t1;
        return get_unsigned(t1);
    }

}

_Bool iscompatible(Type t1, Type t2) {
    t1 = unqual(t1); t2 = unqual(t2);
    if (t1 == t2)
        return 1;
    // incomplete todo
}

