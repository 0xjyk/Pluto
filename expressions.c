#include "pluto.h"
#include "include/parse.h"

// expressions
Node primary_expression(){
    // identifier
    // constant
    // string literal
    // ( expression )
    // generic_selection

    Token tok = lex();
    Node nd;
    switch (tok->type) {
        case ID:
            nd = make_node(ND_ID, PERM);
            nd->val.strval = tok->val.strval;
            // assign type based on enclosing declaration 
            Symbol sym = lookup(nd->val.strval, identifiers);
            nd->lval = 1;
            nd->rval = 1;
            if (!sym) {
                error(&tok->loc, "usage of identifier before declaration");
                nd->sym = install(nd->val.strval, &identifiers, level, level < LOCAL ? PERM : FUNC);
                nd->sym->type = sinttype;
                nd->type = sinttype;
            } else {
                // identifiers that have func type are understood to ptr to corresponding func type
                if (isfunc(sym->type)) {
                    sym->type = make_ptr(sym->type);
                    nd->lval = 0;
                } 
                // arrays are converted to pointers
                if (isarray(sym->type)) 
                    sym->type = atop(sym->type);
                nd->type = sym->type;
                nd->sym = sym;
            }
            nd->loc = tok->loc;
            return nd;
        case UCHAR:
            nd = make_node(ND_CONST, PERM);
            nd->type = uchartype;
            nd->subid = ND_UCHAR;
            nd->val.charval.c = tok->val.charval.c;
            nd->loc = tok->loc;
            nd->lval = 0;
            nd->rval = 1;
            return nd;
        case INTCONST:
            nd = make_node(ND_CONST, PERM);
            nd->subid = ND_INT;
            nd->subsubid = tok->subtype;
            nd->loc = tok->loc;
            nd->lval = 0;
            nd->rval = 1;
            switch (tok->subtype) {
                case ULLONG:
                    nd->type = ulonglongtype;
                    nd->val.intval.ull = tok->val.intval.ull; break;
                case SLLONG:
                    nd->type = slonglongtype;
                    nd->val.intval.ll = tok->val.intval.ll; break;
                case ULONG:
                    nd->type = ulongtype;
                    nd->val.intval.ul = tok->val.intval.ul; break;
                case SLONG:
                    nd->type = slongtype;
                    nd->val.intval.l = tok->val.intval.l; break;
                case UINT:
                    nd->type = uinttype;
                    nd->val.intval.u = tok->val.intval.u; break;
                case SINT:
                    nd->type = sinttype;
                    nd->val.intval.i = tok->val.intval.i; break;
                default:
                    // internal error shouldn't reach this branch
                    error(&(tok->loc), "internal error while processing int primary expression");
                    // for error recovery, set err to 1 & store a dummy int val
                    nd->err = 1; nd->type = sinttype; nd->subsubid = SINT; nd->val.intval.i = 1;
                    break;
            }
            return nd;
        case FLOATCONST:
            nd = make_node(ND_CONST, PERM);
            nd->subid = ND_FLOAT;
            nd->subsubid = tok->subtype;
            nd->loc = tok->loc;
            nd->lval = 0;
            nd->rval = 1;
            switch (tok->subtype) {
                case SDOUBLE:
                    nd->type = doubletype;
                    nd->val.floatval.d = tok->val.floatval.d; break;
                case LDOUBLE:
                    nd->type = longdoubletype;
                    nd->val.floatval.ld = tok->val.floatval.ld; break;
                case SFLOAT:
                    nd->type = floattype;
                    nd->val.floatval.f = tok->val.floatval.f; break;
                default:
                    // internal error shoudn't reach this branch
                    error(&(tok->loc), "internal error while processing float primary expression");
                    nd->err = 1; nd->type = floattype; nd->subsubid = SFLOAT; nd->val.floatval.f = 1;
                    break;
            }
            return nd;
        case STR:
            nd = make_node(ND_STR, PERM);
            nd->type = charptype;
            nd->val.strval = tok->val.strval;
            nd->loc = tok->loc;
            nd->lval = 1;
            nd->rval = 1;
            return nd;
        case PUNCT:
            if (tok->subtype != LBRAC) {
                error(&(tok->loc), "expected '(' before sub-expresion");
                // escape till FIRST(expr)
                escape_first(ND_EXPR);
            }
            nd = expression();
            if ((tok = lex())->type != PUNCT || tok->subtype != RBRAC) {
                error(&(tok->loc), "expected ')' after sub-expression");
                while ((tok = lex())->type != PUNCT || tok->subtype != RBRAC) {};
            }
            return nd;
        case KEYWORD:
            if (tok->subtype != _GENERIC) {
                // error - only permissible keyword here is _Generic
                error(&(tok->loc), "expected _Generic keywords before generic expression");
                // insert generic token
                token t = {.type = KEYWORD, .subtype=_GENERIC, .len=0, .loc=tok->loc};
                tok = make_token(t);
            }
            restore_tok(&tok);
            return generic_selection();
        case ERROR:
            restore_tok(&tok);
            nd = make_error_recovery_node();
            return nd;
    }
}


Node generic_selection(){
    // _Generic ( assignment_expression , generic_assoc_list )
    Node gen = make_node(ND_GENERIC, PERM);
    Token tok = lex();
    gen->loc = tok->loc;
    if (tok->type != KEYWORD || tok->subtype != _GENERIC) {
        error(&(tok->loc), "expected _Generic keywords before generic expression");
    }
    if ((tok = lex())->subtype != LBRAC) {
        error(&(tok->loc), "expected '(' before generic expression");
        escape_first(ND_ASSIGNEXPR);
    }
    Node assign_expr = assignment_expression();
    add_child(gen, &assign_expr);
    if ((tok = lex())->subtype != COM) {
        error(&(tok->loc), "expected ',' between the asssignment expression and \
                generic association list");
        // TODO: add escape_first(gen_assoc_list);
    }
    Node gal = generic_assoc_list();
    add_child(gen, &gal);
    if ((tok = lex())->subtype != RBRAC) {
        error(&(tok->loc), "expected ')' after generic expression");
        while ((tok = lex())->subtype != RBRAC) {};
    }
    return gen;
}
Node generic_assoc_list(){
    // generic_association
    // generic_assoc_list , generic_association
    Node gen_ls = make_node(ND_GENLS, PERM);
    Token tok;
    do {
        Node gen_assoc = generic_association();
        add_child(gen_ls, &gen_assoc);
        if (gen_ls->num_kids == 1) 
            gen_ls->loc = gen_assoc->loc;

    } while ((tok = lex())->subtype == COM);
    restore_tok(&tok);
    if (!gen_ls->kids) {
        error(&tok->loc, "expected atleast one generic association statement in generic expression");
    }
    return gen_ls;
}
Node generic_association(){
    // type_name : assignment_expression
    // default : assignment_expression
    Token tok = lex(); 
    Node gen_assoc = make_node(ND_GEN_ASSOC, PERM);
    gen_assoc->loc = tok->loc;
    // extract type
    gen_assoc->type = defaulttype;
    if (tok->subtype != DEFAULT) {
        restore_tok(&tok);
        gen_assoc->type = type_name();
    } 
    tok = lex(); 
    if (tok->subtype != COL) {
        restore_tok(&tok); 
        error(&tok->loc, "exprected ':' before assignment expression in generic expression");
    }
    // extract assignment
    Node assign_expr = assignment_expression();
    add_child(gen_assoc, &assign_expr);
    return gen_assoc;
}

Node postfix_expression(){
    // primary_expression
    // postfix_expression [ expression ]
    // postfix_expression ( argument_expression_list )
    // postfix_expression . identifier
    // postfix_expression -> identifier
    // postfix_expression ++
    // postfix_expression --
    // ( type_name ) { initializer_list }   * (type_name ) would have already been extracted by cast_expreesion *
    // ( type_name ) { initializer_list , } * (type_name ) would have already been extracted by cast_expreesion *
    Token tok = lex();
    Node pf_root;
    if (tok->subtype == LCBRAC) {
        pf_root = make_node(ND_COMP_LIT, PERM);
        pf_root->loc = tok->loc;
        Node init_list = initializer_list();
        add_child(pf_root, &init_list);
        if ((tok = lex())->subtype != RCBRAC) {
                error(&tok->loc, "expected closing '}' after initializer list in compound literal");
                restore_tok(&tok);
        }
    } else {
        restore_tok(&tok);
        pf_root = primary_expression();
    }


/*

    // could be either a compound literal or ( expression )
    if (tok->subtype == LBRAC) {
        Token tokcpy = tok;
        tok = lex(); 
        if (!first(ND_TYPENAME, tok)) {
            // restore in opposite order of retrieval
            restore_tok(&tok);
            restore_tok(&tokcpy);
            pf_root = primary_expression();
        } else {
            // type_name
            pf_root = make_node(ND_COMP_LIT, PERM);
            restore_tok(&tok);
            pf_root->type = type_name();
            if ((tok = lex())->subtype != RBRAC) {
                restore_tok(&tok);
                error(&tok->loc, "expected closing ')' after type name in compound literal");
            }
            // { initializer_list }
            if ((tok = lex())->subtype != LCBRAC) {
                error(&tok->loc, "expected opening '(' before initializer list in compound literal");
                restore_tok(&tok);
            } 
            Node init_list = initializer_list();
            if ((tok = lex())->subtype != RCBRAC) {
                error(&tok->loc, "expected closing '}' after initializer list in compound literal");
                restore_tok(&tok);
            }
            add_child(pf_root, &init_list);
        }
    } else {
        restore_tok(&tok);
        pf_root = primary_expression();
    }
*/
    while (1) {
        tok = lex();
        if (tok->type != PUNCT) {
            restore_tok(&tok);
            break;
        }
        switch (tok->subtype) {
            case LSQBRAC:
                {
                Node sel = make_node(ND_SELECT, PERM);
                sel->loc = tok->loc;
                add_child(sel, &pf_root);
                // ensure that pf root has array or pointer type
                if (!isarray(pf_root->type) && !isptr(pf_root->type)) {
                    error(&tok->loc, "array subscripting operand expects array/pointer type");
                    pf_root->type = make_array(pf_root->type, 5, 0);
                }
                sel->type = array_type(pf_root->type);
                Node expr = expression();
                // type check exr and ensure that it has type integer
                add_child(sel, &expr);
                sel->lval = 1;
                sel->rval = 1;
                pf_root = sel;
                if ((tok = lex())->type != PUNCT || tok->subtype != RSQBRAC) {
                    error(&(tok->loc), "expected closing ']' after expression in seclection expression");
                    while ((tok = lex())->type != PUNCT || tok->subtype != RSQBRAC) {};
                }
                break;
                }
            case LBRAC:
                {
                Node call = make_node(ND_CALL, PERM);
                call->loc = tok->loc;
                add_child(call, &pf_root);
                // ensure that pf_root has ptr to function type
                if (!isptr(pf_root->type) || !isfunc(deref(pf_root->type))) {
                    error(&tok->loc, 
                    "operand to a function call should have function or pointer to function type");
                    call->type = sinttype;
                    tc.call_err = 1;
                } else 
                    call->type = deref(pf_root->type)->type;

                if ((tok = lex())->type == PUNCT && tok->subtype == RBRAC) {
                    if (!tc.call_err)
                        expect_noargs(deref(pf_root->type));
                    tc.call_err = 0;
                    pf_root = call;
                    break;
                }
                restore_tok(&tok);
                Node arg_expr_list = argument_expression_list();
                if (!tc.call_err) 
                    match_proto(arg_expr_list, deref(pf_root->type));
                tc.call_err = 0;
                add_child(call, &arg_expr_list);
                call->lval = 0;
                call->rval = 1;
                pf_root = call;
                if ((tok = lex())->type != PUNCT || tok->subtype != RBRAC) {
                    error(&(tok->loc), "expected closing ')' after expression in function call expression");
                    while ((tok = lex())->type != PUNCT || tok->subtype != RBRAC) {};
                }
                break;
                }
            case DOT:
                {
                Node dot = make_node(ND_DOT, PERM);
                dot->loc = tok->loc;
                add_child(dot, &pf_root);
                if (pf_root->lval)
                    dot->lval = 1;
                dot->rval = 1;
                // ensure that pf_root has struct/union type
                if (!isstructunion(pf_root->type)) {
                    error(&tok->loc, "expected first operand of '.' operator to have struct/union type");
                    tc.su_err =  1;
                }

                Node id = make_node(ND_ID, PERM);
                tok = lex();
                id->loc = tok->loc;
                if (tok->type != ID) {
                    error(&(tok->loc), "expected identifier after '.' operator");
                    tc.su_err = 1;
                }
                if (!tc.su_err) {
                    id->field = get_member(pf_root->type, tok->val.strval); 
                    dot->field = id->field;
                    if (!id->field) 
                        error(&tok->loc, "given member does not belong to the given struct/union");
                    else {
                        // extra indirection is used in enforce C's cascading types
                        //dot->type = id->field->type;
                        //id->type = id->field->type;
                        dot->type = fieldtype(pf_root->type, id->field->type);
                        id->type = dot->type;
                    }

                }
                tc.su_err = 0;
                id->val.strval = tok->val.strval;
                add_child(dot, &id);
                pf_root = dot;
                break;
                }
            case ARROW:
                {
                Node arr = make_node(ND_ARROW, PERM);
                arr->loc = tok->loc;
                add_child(arr, &pf_root);
                arr->lval = 1;
                arr->rval = 1;
                // ensure that pf_root has ptr to struct/union type
                if (!isptr(pf_root->type) || !isstructunion(deref(pf_root->type))) {
                    error(&tok->loc, 
                            "expected first operand of '->' operator to have pointer to struct/union type");
                    tc.su_err = 1;
                }
                Node idd = make_node(ND_ID, PERM);
                tok = lex();
                idd->loc = tok->loc;
                if (tok->type != ID) {
                    error(&(tok->loc), "expected identifier after '->' operator");
                    tc.su_err = 1;
                }
                if (!tc.su_err) {
                    idd->field = get_member(deref(pf_root->type), tok->val.strval);
                    arr->field = idd->field;

                    if (!idd->field)
                        error(&tok->loc, "given member does not belong to the given struct/union");
                    else {
                        //arr->type = idd->field->type;
                        //idd->type = idd->field->type;
                        arr->type = fieldtype(pf_root->type, idd->field->type);
                        idd->type = arr->type; 
                    }
                }
                tc.su_err = 0;
                idd->val.strval = tok->val.strval;
                add_child(arr, &idd);
                pf_root = arr;
                break;
                }
            case INCR:
                {
                Node incr = make_node(ND_INCR, PERM);
                incr->loc = tok->loc;
                add_child(incr, &pf_root);
                incr->type = pf_root->type;
                incr->lval = 0;
                incr->rval = 1;
                if (!isptr(pf_root->type) && !isarith(pf_root->type))
                    error(&tok->loc, "increment operator requires real type / pointer type");
                // first operand should be both an lval and modifiable
                else if (!pf_root->lval || !modifiable_lval(pf_root->type)) 
                    error(&tok->loc, "modifiable lvalue required as increment operand");
                pf_root = incr;
                break;
                }
            case DECR:
                {
                Node decr = make_node(ND_DECR, PERM);
                decr->loc = tok->loc;
                add_child(decr, &pf_root);
                decr->type = pf_root->type;
                decr->lval = 0;
                decr->rval = 1;
                if (!isptr(pf_root->type) && !isarith(pf_root->type))
                    error(&tok->loc, "decrement operator requires real type / pointer type");
                // first operand should be both an lval and modifiable
                else if (!pf_root->lval || !modifiable_lval(pf_root->type)) 
                    error(&tok->loc, "modifiable lvalue required as decrement operand");
                pf_root = decr;
                break;
                }
            default:
                restore_tok(&tok);
                return pf_root;
        }
    }
    return pf_root;
}

Type type_cast() {
    // ( type_name )
    Token tok = lex();
    Type tc = NULL;
    // could be either a compound literal or ( expression )
    if (tok->subtype == LBRAC) {
        Token tokcpy = tok;
        tok = lex(); 
        if (!first(ND_TYPENAME, tok)) {
            // restore in opposite order of retrieval
            restore_tok(&tok);
            restore_tok(&tokcpy);
        } else {
            // type_name
            restore_tok(&tok);
            tc = type_name();
            if ((tok = lex())->subtype != RBRAC) {
                restore_tok(&tok);
                error(&tok->loc, "expected closing ')' after type name in compound literal");
            }
        }
    } else
        restore_tok(&tok);
    return tc;
}

/*
Node argument_expression_list(){
    // assignment_expression
    // argument_expression_list , assignment_expression
    Node arg_expr = assignment_expression();
    Token tok = lex();
    if (tok->type != PUNCT || tok->subtype != COM) {
        restore_tok(&tok);
        return arg_expr;
    }
    restore_tok(&tok);
    Node arg_ls = make_node(ND_ARG_LIST, PERM);
    add_child(arg_ls, &arg_expr);
    arg_ls->loc = arg_expr->loc;
    while ((tok = lex())->type == PUNCT && tok->subtype == COM) {
        arg_expr = assignment_expression();
        add_child(arg_ls, &arg_expr);
    }
    restore_tok(&tok);
    return arg_ls;
}
*/

Node argument_expression_list(){
    // assignment_expression
    // argument_expression_list , assignment_expression
    Node arg_ls = make_node(ND_ARG_LIST, PERM);
    Node assign_expr;
    Token tok;
    do {
        assign_expr = assignment_expression();
        add_child(arg_ls, &assign_expr);
        if (arg_ls->num_kids == 1) 
            arg_ls->loc = assign_expr->loc;
    } while ((tok = lex())->subtype == COM);
    restore_tok(&tok);
    return arg_ls;
}
Node unary_expression(){
    // postfix-expression
    // ++ unary_expression
    // -- unary_expression
    // unary_operator cast_expression
    // sizeof unary_expression
    // sizeof ( type_name )
    // _Alignof ( type_name )
    Token tok = lex();
    Node nd;
    switch (tok->type) {
        case ID: case STR: case INTCONST:
        case FLOATCONST: case UCHAR:
            restore_tok(&tok);
            return postfix_expression();
        case PUNCT:
            nd = make_node(0, PERM);
            nd->loc = tok->loc;
            Node temp;
            switch (tok->subtype) {
                case LBRAC: case LCBRAC:
                    restore_tok(&tok);
                    return postfix_expression();
                case INCR:
                    nd->id = ND_UINCR;
                    temp = unary_expression();
                    nd->type = temp->type;
                    nd->lval = 0; 
                    nd->rval = 1;
                    if (!isptr(temp->type) && !isarith(temp->type))
                        error(&nd->loc, "increment operator requires real type / pointer type");
                    else if (!temp->lval || !modifiable_lval(temp->type)) 
                        error(&nd->loc, "modifiable lvalue required as increment operand");
                    add_child(nd, &temp);
                    return nd;
                case DECR:
                    nd->id = ND_UDECR;
                    temp = unary_expression();
                    nd->type = temp->type;
                    nd->lval = 0; 
                    nd->rval = 1;
                    if (!isptr(temp->type) && !isarith(temp->type))
                        error(&nd->loc, "increment operator requires real type / pointer type");
                    else if (!temp->lval || !modifiable_lval(temp->type)) 
                        error(&nd->loc, "modifiable lvalue required as increment operand");
                    add_child(nd, &temp);
                    return nd;
                case '&':
                    nd->id = ND_UAND;
                    temp = cast_expression();
                    nd->lval = 0;
                    nd->rval = 1;
                      
                    if (temp->id != ND_SELECT && temp->id != ND_UDEREF && 
                        !temp->lval && !(isptr(temp->type) && isfunc(deref(temp->type))))
                        error(&nd->loc, "invalid operad to '&' operator");

                    // also an error if field with bit size is specified
                    if (temp->field && temp->field->bitsize) 
                        error(&nd->loc, "invalid operad to '&' operator");

                    // special case for functions
                    if (isptr(temp->type) && isfunc(deref(temp->type)))
                        nd->type = temp->type;
                    else 
                        nd->type = make_ptr(temp->type);

                    add_child(nd, &temp);
                    return nd;
                case '*':
                    nd->id = ND_UDEREF;
                    temp = cast_expression();
                    nd->lval = 1;
                    nd->rval = 1;
                    if (!isptr(temp->type)) {
                        error(&nd->loc, "dereference operator requires operand of pointer type");
                        nd->type = sinttype;
                    } else {
                        if (isfunc(deref(temp->type))) {
                            nd->type = temp->type;
                            nd->lval = 0;
                        } else 
                            nd->type = deref(temp->type);
                    }
                    add_child(nd, &temp);
                    return nd;
                case '+':
                    nd->id = ND_UPLUS;
                    temp = cast_expression();
                    nd->lval = 0;
                    nd->rval = 1;
                    if (!isarith(temp->type)) {
                        error(&nd->loc, "unary '+' operator requires operand of arithmetic type");
                        nd->type = sinttype;
                    } else 
                        nd->type = promote(temp->type);
                    add_child(nd, &temp);
                    return nd;
                case '-':
                    nd->id = ND_UMINUS;
                    temp = cast_expression();
                    nd->lval = 0;
                    nd->rval = 1;
                    if (!isarith(temp->type)) {
                        error(&nd->loc, "unary '-' operator requires operand of arithmetic type");
                        nd->type = sinttype;
                    } else 
                        nd->type = promote(temp->type);

                    add_child(nd, &temp);
                    return nd;
                case '~':
                    nd->id = ND_UBITNOT;
                    temp = cast_expression();
                    nd->lval = 0;
                    nd->rval = 1;
                    if (!isint(temp->type)) {
                        error(&nd->loc, "unary '~' operator requires operand of integer type");
                        nd->type = sinttype;
                    } else {
                        // first integer promotion is applied to the operand
                        // the resulting type is the promoted type
                        temp->type = promote(temp->type);
                        nd->type = promote(temp->type);
                    }

                    add_child(nd, &temp);
                    return nd;
                case '!':
                    nd->id = ND_UNOT;
                    temp = cast_expression();
                    nd->lval = 0;
                    nd->rval = 1;
                    if (!isscalar(temp->type)) {
                        error(&nd->loc, "unary '!' operator requires operand of scalar type");
                        nd->type = sinttype;
                    } else 
                        nd->type = sinttype;

                    add_child(nd, &temp);
                    return nd;
                default:
                    error(&tok->loc, "unknown unaray expression");
                    // escape till FOLLOW(unary_expression)
                    nd->lval = 0; 
                    nd->rval = 1;
                    nd->id = ND_CONST;
                    nd->subid = ND_INT;
                    nd->type = sinttype;
                    return nd;

            }
        case KEYWORD:
            switch (tok->subtype) {
                case SIZEOF:
                    nd = make_node(ND_SIZEOF, PERM);
                    nd->type = uinttype;
                    nd->loc = tok->loc;
                    nd->lval = 0; 
                    nd->rval = 1;
                    Type tc = type_cast(); 
                    tok = lex();
                    if (first(ND_UNARY, tok)) {
                        restore_tok(&tok);
                        temp = unary_expression();
                        // overwrite temp's type with the cast
                        if (tc)
                            temp->type = tc;
                        add_child(nd, &temp);
                    } else  {
                        restore_tok(&tok);
                        temp = make_node(ND_TYPENAME, PERM);
                        temp->type = tc;
                        add_child(nd, &temp);
                        temp->loc = nd->loc;
                    } 
                    if ((isptr(temp->type) && isfunc(deref(temp->type))) || 
                        (!iscomplete(temp->type))) {
                        error(&nd->loc, 
                        "sizeof operator cannot be applied to a function type or an incomplete type");
                    }
                    if (temp->field && temp->field->bitsize) {
                        error(&nd->loc, "sizeof operator cannot be applied to a bit-field operand");
                    }
                    return nd;
                case _ALIGNOF:
                    nd = make_node(ND_ALIGNOF, PERM);
                    nd->type = uinttype;
                    nd->loc = tok->loc;
                    nd->lval = 0;
                    nd->rval = 1;
                    temp = make_node(ND_TYPENAME, PERM);
                    temp->type = type_cast();
                    temp->loc = nd->loc;
                    if (!temp->type) {
                        error(&tok->loc, "expected '(' type name ')' after _Alignof operator");
                        temp->type = sinttype;
                    } else {
                        if ((isptr(temp->type) && isfunc(deref(temp->type))) || 
                            (!iscomplete(temp->type))) {
                            error(&nd->loc, 
                            "sizeof operator cannot be applied to a function type or an incomplete type");
                        }
                    } 
                    add_child(nd, &temp);

                    return nd; 
                case _GENERIC:
                    restore_tok(&tok);
                    return postfix_expression();
                default:
                    error(&tok->loc, "unknown unaray expression");
                    // escape till FOLLOW(unary_expression)
                    nd->lval = 0;
                    nd->rval = 1;
                    nd->id = ND_CONST;
                    nd->subid = ND_INT;
                    nd->type = sinttype;
                    return nd;
            }
        default:
            error(&tok->loc, "unknown unaray expression");
            // escape till FOLLOW(unary_expression)
            nd->lval = 0;
            nd->rval = 1;
            nd->id = ND_CONST;
            nd->subid = ND_INT;
            nd->type = sinttype;
            return nd;
    }

}
Node cast_expression(){
    // unary_expression
    // ( type_name ) cast_expression
    Token tok = lex();
    Type tc = NULL;
    if (tok->type == PUNCT && tok->subtype == LBRAC) {
        // process type-name ..
        restore_tok(&tok);
        tc = type_cast();
        if (tc) {
            if (tc != voidtype && !isscalar(tc)) {
                error(&tok->loc, "type case shall either be of void type or scalar type");
            }
            Node ce = cast_expression();
            if (!isscalar(ce->type)) {
                error(&tok->loc, "operand of a type case is required to have scalar type");
            }
            if ((isptr(tc) && isdouble(ce->type)) || 
                (isdouble(tc) && isptr(ce->type))) {
                error(&tok->loc, "float to pointer / pointer to float conversions are not permitted");
            }
            ce->type = tc;
            ce->lval = 0;
            ce->rval = 1;
            return ce;
        }   
    } else 
        restore_tok(&tok);
    // unary_expression
    return unary_expression();
}
Node multiplicative_expression(){
    // cast_expression
    // multiplicative_expression * cast_expression
    // multiplicative_expression / cast_expression
    // multiplicative_expression % cast_expression
    Node l = cast_expression();
    Token tok;
    while ((tok = lex())->type == PUNCT &&
            (tok->subtype == STAR || tok->subtype == DIV ||
             tok->subtype == MOD)) {
        if (tok->subtype == STAR) {
            Node mult_expr = make_node(ND_MULT, PERM);
            add_child(mult_expr, &l);
            mult_expr->loc = tok->loc;
            Node r = cast_expression();
            add_child(mult_expr, &r);
            if (!isarith(l->type) || !isarith(r->type)) {
                error(&mult_expr->loc, "both operands of the '*' operator should have arithmetic type");
                l->type = sinttype;
                r->type = sinttype;
            }
            mult_expr->type = usual_arithmetic_conversion(l->type, r->type);
            l = mult_expr;
        } else if (tok->subtype == DIV) {
            Node div_expr = make_node(ND_DIV, PERM);
            add_child(div_expr, &l);
            div_expr->loc = tok->loc;
            Node r = cast_expression();
            add_child(div_expr, &r);
            if (!isarith(l->type) || !isarith(r->type))  {
                error(&div_expr->loc, "both operands of the '/' operator should have arithmetic type");
                l->type = sinttype;
                r->type = sinttype;
            }
            div_expr->type = usual_arithmetic_conversion(l->type, r->type);
            l = div_expr;
        } else {
            Node mod_expr = make_node(ND_MOD, PERM);
            add_child(mod_expr, &l);
            mod_expr->loc = tok->loc;
            Node r = cast_expression();
            add_child(mod_expr, &r);
            if (!isint(l->type) || !isint(r->type)) {
                error(&mod_expr->loc, "both operands of the '%' operator should have integer type");
                l->type = sinttype;
                r->type = sinttype;
            }
            mod_expr->type = usual_arithmetic_conversion(l->type, r->type);
            l = mod_expr;
        }
        l->lval = 0; 
        l->rval = 1;
    }
    restore_tok(&tok);
    return l;
}
Node additive_expression(){
    // multiplicative_expression
    // additive_expression + multiplicative_expression
    // additive_expression - multiplicative_expression
    Node l = multiplicative_expression();
    Token tok;
    while ((tok = lex())->type == PUNCT &&
            (tok->subtype == PLUS || tok->subtype == NEG)) {
        if (tok->subtype == PLUS) {
            Node add_expr = make_node(ND_ADD, PERM);
            add_child(add_expr, &l);
            add_expr->loc = tok->loc;
            Node r = multiplicative_expression();
            add_child(add_expr, &r);
            if (isarith(l->type) && isarith(r->type)) {
                // all good 
            } else if (isptr(l->type) && iscomplete(deref(l->type)) && isarith(r->type)) {
                // all good 
            } else if (isarith(l->type) && isptr(r->type) && iscomplete(deref(r->type))) {
                // all good 
            } else {
                // not so good
                error(&add_expr->loc, "invalid operands to '+' operator");
            }
            l = add_expr;
        } else {
            Node sub_expr = make_node(ND_SUB, PERM);
            add_child(sub_expr, &l);
            sub_expr->loc = tok->loc;
            Node r = multiplicative_expression();
            add_child(sub_expr, &r);
            if (isarith(l->type) && isarith(r->type)) {
                // all good
            } else if (isptr(l->type) && isptr(r->type)) {
                // both types should be compatible
                if (!iscomplete(deref(l->type)) || !iscomplete(r->type)) 
                    error(&sub_expr->loc, "invalid operands to '-' operator");
                else if (!iscompatible(deref(l->type), deref(r->type))) 
                    error(&sub_expr->loc, "invalid operands to '-' operator");
                // else all good 
            } else if (isptr(l->type) && isint(r->type)) {
                // all good 
            } else {
                // not so good 
                error(&sub_expr->loc, "invalid operands to '-' operator");
            }
            l = sub_expr;
        }
    }
    restore_tok(&tok);
    return l;
}
Node shift_expression(){
    // additive_expression
    // shift_expresion << additive_expression
    // shift_expression >> additive_expression
    Node l = additive_expression();
    Token tok;
    while ((tok = lex())->type == PUNCT &&
            (tok->subtype == LSHFT || tok->subtype == RSHFT)) {
        if (tok->subtype == LSHFT) {
            Node lshft_expr = make_node(ND_LSHFT, PERM);
            add_child(lshft_expr, &l);
            lshft_expr->loc = tok->loc;
            Node r = additive_expression();
            add_child(lshft_expr, &r);
            l = lshft_expr;
        } else {
            Node rshft_expr = make_node(ND_RSHFT, PERM);
            add_child(rshft_expr, &l);
            rshft_expr->loc = tok->loc;
            Node r = additive_expression();
            add_child(rshft_expr, &r);
            l = rshft_expr;
        }
    }
    restore_tok(&tok);
    return l;
}
Node relational_expression(){
    // shift_expression
    // relational_expression < shift_expression
    // relational_expression > shift_expression
    // relational_expression <= shift_expression
    // relational_expression >= shift_expression
    Node l = shift_expression();
    Token tok;
    while ((tok = lex())->type == PUNCT &&
            (tok->subtype == LESS || tok->subtype == GREATER ||
             tok->subtype == LEQ || tok->subtype == GEQ)) {
        if (tok->subtype == LESS) {
            Node less_expr = make_node(ND_LESS, PERM);
            add_child(less_expr, &l);
            less_expr->loc = tok->loc;
            Node r = shift_expression();
            add_child(less_expr, &r);
            l = less_expr;
        } else if (tok->subtype == GREATER) {
            Node greater_expr = make_node(ND_GREATER, PERM);
            add_child(greater_expr, &l);
            greater_expr->loc = tok->loc;
            Node r = shift_expression();
            add_child(greater_expr, &r);
            l = greater_expr;
        } else if (tok->subtype == LEQ) {
            Node leq_expr = make_node(ND_LEQ, PERM);
            add_child(leq_expr, &l);
            leq_expr->loc = tok->loc;
            Node r = shift_expression();
            add_child(leq_expr, &r);
            l = leq_expr;
        } else {
            Node geq_expr = make_node(ND_GEQ, PERM);
            add_child(geq_expr, &l);
            geq_expr->loc = tok->loc;
            Node r = shift_expression();
            add_child(geq_expr, &r);
            l = geq_expr;
        }
    }
    restore_tok(&tok);
    return l;
}
Node equality_expression(){
    // relational_expression
    // equality_expression == relational_expression
    // equality_expression != relational_expression
    Node l = relational_expression();
    Token tok;
    while ((tok = lex())->type == PUNCT &&
                (tok->subtype == EQ || tok->subtype == NEQ)) {
        if (tok->subtype == EQ) {
            Node eq_expr = make_node(ND_EQ, PERM);
            add_child(eq_expr, &l);
            eq_expr->loc = tok->loc;
            Node r = relational_expression();
            add_child(eq_expr, &r);
            l = eq_expr;
        } else {
            Node neq_expr = make_node(ND_NEQ, PERM);
            add_child(neq_expr, &l);
            neq_expr->loc = tok->loc;
            Node r = relational_expression();
            add_child(neq_expr, &r);
            l = neq_expr;
        }

    }
    restore_tok(&tok);
    return l;
}
Node AND_expression(){
    // equality_expression
    // AND_expression & equality_expression
    Node l = equality_expression();
    Token tok;
    while ((tok = lex())->type == PUNCT && tok->subtype == BAND) {
        Node band_expr = make_node(ND_BAND, PERM);
        add_child(band_expr, &l);
        band_expr->loc = tok->loc;
        Node r = equality_expression();
        add_child(band_expr, &r);
        l = band_expr;
    }
    restore_tok(&tok);
    return l;
}
Node exclusive_OR_expression(){
    // AND_expression
    // exclusive_OR_expression ^ AND_expression
    Node l = AND_expression();
    Token tok;
    while ((tok = lex())->type == PUNCT && tok->subtype == XOR) {
        Node xor_expr = make_node(ND_XOR, PERM);
        add_child(xor_expr, &l);
        xor_expr->loc = tok->loc;
        Node r = exclusive_OR_expression();
        add_child(xor_expr, &r);
        l = xor_expr;
    }
    restore_tok(&tok);
    return l;
}
Node inclusive_OR_expression(){
    // exclusive_OR_expression
    // inclusive_OR_expression | exclusive_OR_expression
    Node l =  exclusive_OR_expression();
    Token tok;
    while ((tok = lex())->type == PUNCT && tok->subtype == BOR) {
        Node ior_expr = make_node(ND_IOR, PERM);
        add_child(ior_expr, &l);
        ior_expr->loc = tok->loc;
        Node r = exclusive_OR_expression();
        add_child(ior_expr, &r);
        l = ior_expr;
    }
    restore_tok(&tok);
    return l;
}
Node logical_AND_expression(){
    // inclusive_OR_expression
    // logical_AND_expression && inclusive_OR_expression
    Node l = inclusive_OR_expression();
    Token tok;
    while ((tok = lex())->type == PUNCT && tok->subtype == AND) {
        Node and_expr = make_node(ND_AND, PERM);
        add_child(and_expr, &l);
        and_expr->loc = tok->loc;
        Node r = inclusive_OR_expression();
        add_child(and_expr, &r);
        l = and_expr;
    }
    restore_tok(&tok);
    return l;
}
Node logical_OR_expression(){
    // logical_AND_expression
    // logical_OR_expression || logical_AND_expression
    Node l = logical_AND_expression();
    Token tok;
    while ((tok = lex())->type == PUNCT && tok->subtype == OR) {
        Node or_expr = make_node(ND_OR, PERM);
        add_child(or_expr, &l);
        or_expr->loc = tok->loc;
        Node r = logical_AND_expression();
        add_child(or_expr, &r);
        l = or_expr;
    }
    restore_tok(&tok);
    return l;
}
Node conditional_expression(){
    // logical_OR_expression
    // logical_OR_expression ? expression : conditional_expression
    Node pred = logical_OR_expression();
    Token tok;
    if ((tok = lex())->type == PUNCT && tok->subtype == Q) {
        Node cond_expr = make_node(ND_COND, PERM);
        add_child(cond_expr, &pred);
        cond_expr->loc = tok->loc;
        Node expr1 = expression();
        add_child(cond_expr, &expr1);
        if ((tok = lex())->type != PUNCT || tok->subtype != COL) {
            // error required ':'
        }
        Node expr2 = conditional_expression();
        add_child(cond_expr, &expr2);
        return cond_expr;
    }
    restore_tok(&tok);
    return pred;
}
Node assignment_expression(){
    // conditional_expression
    // unary_expression assignment_operator assignment_expression
    Node l = conditional_expression();
    if (l->id <= ND_UNARY) {
        Token tok = lex();
        if (tok->type != PUNCT) {
            restore_tok(&tok);
            return l;
        }
        Node assign_expr = make_node(ND_ASSIGNEXPR, PERM);
        assign_expr->loc = tok->loc;
        add_child(assign_expr, &l);
        Node r;
        switch (tok->subtype) {
            case ASSIGN:
                assign_expr->subid = ND_ASSIGN;
                r = assignment_expression();
                add_child(assign_expr, &r);
                return assign_expr;
            case MULTEQ:
                assign_expr->subid = ND_MULT_ASSIGN;
                r = assignment_expression();
                add_child(assign_expr, &r);
                return assign_expr;

            case DIVEQ:
                assign_expr->subid = ND_DIV_ASSIGN;
                r = assignment_expression();
                add_child(assign_expr, &r);
                return assign_expr;
            case MODEQ:
                assign_expr->subid = ND_MOD_ASSIGN;
                r = assignment_expression();
                add_child(assign_expr, &r);
                return assign_expr;
            case INCREQ:
                assign_expr->subid = ND_INCR_ASSIGN;
                r = assignment_expression();
                add_child(assign_expr, &r);
                return assign_expr;
            case DECREQ:
                assign_expr->subid = ND_DECR_ASSIGN;
                r = assignment_expression();
                add_child(assign_expr, &r);
                return assign_expr;
            case LSHFTEQ:
                assign_expr->subid = ND_LSHFT_ASSIGN;
                r = assignment_expression();
                add_child(assign_expr, &r);
                return assign_expr;
            case RSHFTEQ:
                assign_expr->subid = ND_RSHFT_ASSIGN;
                r = assignment_expression();
                add_child(assign_expr, &r);
                return assign_expr;
            case ANDEQ:
                assign_expr->subid = ND_AND_ASSIGN;
                r = assignment_expression();
                add_child(assign_expr, &r);
                return assign_expr;
            case XOREQ:
                assign_expr->subid = ND_XOR_ASSIGN;
                r = assignment_expression();
                add_child(assign_expr, &r);
                return assign_expr;
            case OREQ:
                assign_expr->subid = ND_OR_ASSIGN;
                r = assignment_expression();
                add_child(assign_expr, &r);
                return assign_expr;
            default:
                restore_tok(&tok);
                return l;
        }
        r = assignment_expression();
        add_child(assign_expr, &r);
        return assign_expr;
    }
    return l;
}

Node expression(){
    // assignment_expression
    // expression , assignment_expression
    Node assign_expr = assignment_expression();
    Token tok = lex();
    if (tok->type != PUNCT || tok->subtype != COM) {
        restore_tok(&tok);
        return assign_expr;
    }
    restore_tok(&tok);
    Node expr = make_node(ND_EXPR, PERM);
    add_child(expr, &assign_expr);
    expr->loc = assign_expr->loc;
    while ((tok = lex())->type == PUNCT && tok->subtype == COM) {
        assign_expr = assignment_expression();
        add_child(expr, &assign_expr);
    }
    restore_tok(&tok);
    return expr;
}

Node constant_expression(){
    return conditional_expression();
}

Node enumeration_constant(){}
