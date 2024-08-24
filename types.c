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
    if (id != FUNCTION && (id != ARRAY || size > 0)) 
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

/*
char *ttos(Type t) {
    char *str = alloc(100, PERM);
    *str = '\0';
    while (t) {
        if (isptr(t) && isqual(t)) {
            strcat(str, ttos(t->type->type));
            strcat(str, "*");
            strcat(str, t->strrep);
            return str;
        } else if (isptr(t)) {
            strcat(str, ttos(t->type));
            strcat(str, "*");
            return str;
        } else {
            strcat(str, t->strrep);
            strcat(str, " ");
        }
        t = t->type;
    }
    return str;
}
*/

char *ttos(Type t) {
    char *str = alloc(100, PERM);
    *str = '\0';
    _Bool ls = 0;
    while (t) {
        if (isptr(t) && isqual(t)) {
            strcat(str, ttos(t->type));
            strcat(str, " ");
            strcat(str, t->strrep);
            return str;
        } else if (isptr(t)) {
            int cnt = 1;
            while (t->type->id == POINTER) {
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
    return make_type(POINTER, t, 64, 64, pointersym, make_string("*", 1));
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
    return make_type(ARRAY, t, size * t->size, align ? align : t->align, NULL, make_string("array", 5));
}

Type atop(Type t) {
    if (isarray(t))
        return make_ptr(t->type);
    error(&loc, "type error: expected array type");
    return make_ptr(t);
}

Type qual(int id, Type t) {
    // check type qualifier rules: TODO 
    if (id & TQ_ATOMIC) {
        return make_type(_ATOMIC, t, 0, 0, NULL, 
                make_string("_Atomic", 7));
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

Type make_func(Type t, Type *proto) {
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
        int i; 
        for (i = 0; t->u.f.proto[i]; i++)
            ;
        return i > 1 && t->u.f.proto[i-1] == voidtype; 
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
    p->type = make_type(id, NULL, 0, 0, p, make_string("struct", 6));
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
    Type ty = make_type(id, t, size, align, sym, make_string(strrep, strlen(name))); 
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
            if (ty->size == 32) {
                sym->u.limits.max.i = INT_MAX;
                sym->u.limits.min.i = INT_MIN;
            }
            // signed long long
            if (ty->size == 64) {
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
            if (ty->size == 32) 
                sym->u.limits.max.f = FLT_MAX;
            if (ty->size == 64)
                sym->u.limits.max.d = DBL_MAX;
            if (ty->size == 128)
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
    booltype = xxinit(_BOOL, NULL, "_Bool", "_Bool", 8, 8);
    sbooltype = booltype;
    ubooltype = booltype;

    chartype = xxinit(CHAR, NULL, "char", "char", 8, 8); 
    schartype = xxinit(SIGNED, chartype, "signed", "signed char", 8, 8);
    uchartype = xxinit(UNSIGNED, chartype, "unsigned", "unsigned char", 8, 8); 

    shorttype = xxinit(SHORT, NULL, "short", "short", 16, 8);
    sshorttype = xxinit(SIGNED, shorttype, "signed", "signed short", 16, 8);
    ushorttype = xxinit(UNSIGNED, shorttype, "unsigned", "unsgined short", 16, 8);
    
    inttype = xxinit(INT, NULL, "int", "int", 32, 8);
    sinttype = xxinit(SIGNED, inttype, "signed", "signed int", 32, 8);
    uinttype = xxinit(UNSIGNED, inttype, "unsigned", "unsigned int", 32, 8);
    
    longtype = xxinit(LONG, NULL, "long int", "long int", 32, 8); 
    slongtype = xxinit(SIGNED, longtype, "signed", "siged long int", 32, 8);
    ulongtype = xxinit(UNSIGNED, longtype, "unsigned", "unsigned long int", 32, 8);

    longlongtype = xxinit(SLLONG, NULL, "long long int",  "long long int", 64, 8); 
    slonglongtype = xxinit(SIGNED, longlongtype, "signed", "signed long long int", 64, 8);
    ulonglongtype =xxinit(UNSIGNED, longlongtype, "unsigned", "unsigned long long int", 64, 8);

    floattype = xxinit(FLOAT, NULL, "float", "float", 32, 8); 
    doubletype = xxinit(SDOUBLE, NULL, "double", "double", 64, 8);
    longdoubletype = xxinit(LDOUBLE, NULL, "long double", "long double", 128, 8);

    voidtype = xxinit(VOID, NULL, "void", "void", 0, 0);

    pointersym = install(make_string("*", 2), &types, GLOBAL, PERM); 
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







