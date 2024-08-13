#include "pluto.h"
#include "include/parse.h"

Node parse(char *pp_file) {
    // initionalise the buffer
    bufinit(pp_file);
    // initialise the token store
    ts = NULL;

    // root contains 
    Node root = make_node(ND_ROOT, PERM);
    
    // three options - 
    //  function-definition 
    //      declaration-specifiers declarator declaration-list[opt] compound-statement
    //  declaration 
    //      declaration-specifiers init-declarator-list[opt] ;
    //      static_assert_declaration
    Token tok; 
    while ((tok = lex())->type != EOI) {
        if (tok->type == KEYWORD && tok->subtype == _STATIC_ASSERT) {
            restore_tok(&tok);
            Node child = static_assert_declaration();
            add_child(root, &child);
        }
        
    }

    return root;
}


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
            nd->loc = tok->loc;
            return nd;
        case UCHAR: 
            nd = make_node(ND_CONST, PERM); 
            nd->subid = ND_UCHAR;
            nd->val.charval.c = tok->val.charval.c;
            nd->loc = tok->loc;
            return nd;
        case INTCONST: 
            nd = make_node(ND_CONST, PERM); 
            nd->subid = ND_INT;
            nd->subsubid = tok->subtype;
            nd->loc = tok->loc;
            switch (tok->subtype) {
                case ULLONG: 
                    nd->val.intval.ull = tok->val.intval.ull; break;
                case SLLONG: 
                    nd->val.intval.ll = tok->val.intval.ll; break;
                case ULONG:
                    nd->val.intval.ul = tok->val.intval.ul; break; 
                case SLONG:
                    nd->val.intval.l = tok->val.intval.l; break; 
                case UINT:
                    nd->val.intval.u = tok->val.intval.u; break;
                case SINT: 
                    nd->val.intval.i = tok->val.intval.i; break;
                default:
                    // internal error shouldn't reach this branch
                    break;
            }
            return nd;
        case FLOATCONST:
            nd = make_node(ND_CONST, PERM); 
            nd->subid = ND_FLOAT; 
            nd->subsubid = tok->subtype;
            nd->loc = tok->loc;
            switch (tok->subtype) {
                case SDOUBLE: 
                    nd->val.floatval.d = tok->val.floatval.d; break;
                case LDOUBLE:
                    nd->val.floatval.ld = tok->val.floatval.ld; break; 
                case SFLOAT: 
                    nd->val.floatval.f = tok->val.floatval.f; break; 
                default: 
                    // internal error shoudn't reach this branch
                    break;
            }
            return nd;
        case STR:
            nd = make_node(ND_STR, PERM); 
            nd->val.strval = tok->val.strval;
            nd->loc = tok->loc;
            return nd;
        case PUNCT: 
            if (tok->subtype != '(') {
                // error
                // escape till ')'
            }
            nd = expression(); 
            if (!(tok = lex()) || (tok->subtype != ')')) {
                // error required ')'

            }
            return nd;
        case KEYWORD:

            if (tok->subtype != _GENERIC) {
                // error - only permissible keyword here is _Generic 
                
                // error recovery skip content till closing ')'
            }
            break; 
    }

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

    /*
    while (1) {
        Node ch = assignment_expression(); 
        add_child(nd, &ch);
        if ((tok = lex())->type != PUNCT || tok->subtype != ',') {
            restore_tok(&tok);
            break; 
        }
    }
    return nd;
    */
}
Node generic_selection(){
    // _Generic ( assignment_expression , generic_assoc_list )
    Token tok = lex(); 
    if (tok->type != KEYWORD || tok->subtype != _GENERIC) {
        // error expected _Generic
    }
    if ((tok = lex())->type != PUNCT || tok->subtype != '(') {
        // error expected '('
    }
    Node gen = make_node(ND_GENERIC, PERM);
    Node assignexp = assignment_expression(); 
    add_child(gen, &assignexp); 
    if ((tok = lex())->type != PUNCT || tok->subtype != ',') {
        // error expected ','
    }
    Node gal = generic_assoc_list(); 
    add_child(gen, &gal);
    if ((tok = lex())->type != PUNCT || tok->subtype != ')') {
        // error expecteed ')'

    }
    return gen;

}
Node generic_assoc_list(){
    // generic_association 
    // generic_assoc_list , generic_association
    Node nd = make_node(ND_GENLS, PERM);
    Token tok;
    while (1) {
        Node ch = generic_association(); 
        add_child(nd, &ch);
        if ((tok = lex())->type != PUNCT || tok->subtype != ',') {
            restore_tok(&tok);
            break; 
        }
    }
    return nd;


}
Node generic_association(){
    // type_name : assignment_expression
    // default : assignment_expression 

   }
Node type_name(){
    // 
} 

Node postfix_expression(){
    // primary_expression 
    // postfix_expression [ expression ]
    // postfix_expression ( argument_expression_list )
    // postfix_expression . identifier
    // postfix_expression -> identifier
    // postfix_expression ++ 
    // postfix_expression -- 
    // ( type_name ) { initializer_list }
    // ( type_name ) { initializer_list , }
    Token tok = lex(); 
    Node pf_root;
    
    // do (type_name work here : TODO also need to ensure that type-name is followed
    if (tok->type == PUNCT && tok->subtype == '(') {
        
        pf_root = make_node(ND_INIT, PERM);
    return pf_root;
    }
    
    // first parse primary expression
    restore_tok(&tok);
    pf_root = primary_expression();
    
    while (1) {
        tok = lex();
        if (tok->type != PUNCT) {
            restore_tok(&tok);
            break;
        }
        switch (tok->subtype) {
            case '[': 
                {
                Node sel = make_node(ND_SELECT, PERM);
                add_child(sel, &pf_root);
                Node expr = expression(); 
                add_child(sel, &expr);
                pf_root = sel;
                if ((tok = lex())->type != PUNCT || tok->subtype != ']') {
                    // error - expected closing ']'

                }
                break; 
                }
            case '(':
                {
                Node call = make_node(ND_CALL, PERM);
                add_child(call, &pf_root); 
               
                if ((tok = lex())->type == PUNCT && tok->subtype == ')') 
                    break;

                Node arg_expr_list = argument_expression_list(); 
                add_child(call, &arg_expr_list);
                pf_root = call;
                if ((tok = lex())->type != PUNCT || tok->subtype != ')') {
                    // error - expected closing ')'

                }
                break; 
                }
            case '.':
                {
                Node dot = make_node(ND_DOT, PERM);
                add_child(dot, &pf_root); 
                Node id = make_node(ND_ID, PERM);
                tok = lex();
                if (tok != ID) {
                    // error - expected id
                }
                id->val.strval = tok->val.strval;
                add_child(dot, &id); 
                pf_root = id;
                break; 
                }
            case ARROW:
                {
                Node arr = make_node(ND_ARROW, PERM);
                add_child(arr, &pf_root); 
                Node idd = make_node(ND_ID, PERM); 
                tok = lex();
                if (tok != ID) {
                    // erro - expected id
                }
                idd->val.strval = tok->val.strval; 
                add_child(arr, &idd);
                pf_root = idd;
                break; 
                }
            case INCR: 
                {
                Node incr = make_node(ND_INCR, PERM);
                add_child(incr, &pf_root); 
                pf_root = incr; 
                break; 
                }
            case DECR: 
                {
                Node decr = make_node(ND_DECR, PERM);
                add_child(decr, &pf_root); 
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
Node argument_expression_list(){
    // assignment_expression 
    // argument_expression_list , assignment_expression
    Node nd = make_node(ND_ARGEXPR, PERM);
    Token tok;
    while (1) {
        Node ch = assignment_expression(); 
        add_child(nd, &ch);
        if ((tok = lex())->type != PUNCT || tok->subtype != ',') {
            restore_tok(&tok);
            break; 
        }
    }
    return nd;

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
    Node nd = make_node(0, PERM);
    Node temp;
    switch (tok->type) {
        case ID: case STR: case INTCONST: 
        case FLOATCONST: case UCHAR: 
            restore_tok(&tok);
            return postfix_expression(); 
        case PUNCT: 
            switch (tok->subtype) {
                case INCR: 
                    nd->id = ND_UINCR;
                    temp = unary_expression();
                    add_child(nd, &temp); 
                    return nd;
                case DECR: 
                    nd->id = ND_UDECR; 
                    temp = unary_expression();
                    add_child(nd, &temp); 
                    return nd;
                case '&':
                    nd->id = ND_UAND;
                    temp = cast_expression();
                    add_child(nd, &temp);
                    return nd;
                case '*': 
                    nd->id = ND_UDEREF;
                    temp = cast_expression();
                    add_child(nd, &temp);
                    return nd;
                case '+': 
                    nd->id = ND_UPLUS; 
                    temp = cast_expression();
                    add_child(nd, &temp);
                    return nd;
                case '-':
                    nd->id = ND_UMINUS; 
                    temp = cast_expression();
                    add_child(nd, &temp);
                    return nd;
                case '~': 
                    nd->id = ND_UBITNOT; 
                    temp = cast_expression();
                    add_child(nd, &temp);
                    return nd;
                case '!':
                    nd->id = ND_UNOT;
                    temp = cast_expression();
                    add_child(nd, &temp);
                    return nd;
                    // error ? 

            }
        case KEYWORD: 
            switch (tok->subtype) {
                case SIZEOF: 
                    // two possible options - ambiguity

                case _ALIGNOF: 


                case _GENERIC: 
                    restore_tok(&tok);
                    return postfix_expression();
                    // error

            }
        // error 

    }

}
Node cast_expression(){
    // unary_expression 
    // ( type_name ) cast_expression
    Token tok = lex(); 
    if (tok->type == PUNCT && tok->subtype == LBRAC) {
        // process type-name .. 

    }
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
            l = mult_expr; 
        } else if (tok->subtype == DIV) {
            Node div_expr = make_node(ND_DIV, PERM); 
            add_child(div_expr, &l); 
            div_expr->loc = tok->loc;
            Node r = cast_expression(); 
            add_child(div_expr, &r); 
            l = div_expr;
        } else {
            Node mod_expr = make_node(ND_MOD, PERM);
            add_child(mod_expr, &l); 
            mod_expr->loc = tok->loc;
            Node r = cast_expression(); 
            add_child(mod_expr, &r); 
            l = mod_expr;
        }
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
            l = add_expr;
        } else {
            Node sub_expr = make_node(ND_SUB, PERM); 
            add_child(sub_expr, &l); 
            sub_expr->loc = tok->loc;
            Node r = multiplicative_expression(); 
            add_child(sub_expr, &r); 
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
        if ((tok->type != PUNCT) || (tok->subtype != ASSIGN ||
                    tok->subtype != MULTEQ || tok->subtype != DIVEQ ||
                    tok->subtype != MODEQ || tok->subtype != INCREQ ||
                    tok->subtype != DECREQ || tok->subtype != LSHFTEQ ||
                    tok->subtype != RSHFTEQ || tok->subtype != ANDEQ ||
                    tok->subtype != XOREQ || tok->subtype != OREQ)) {
            restore_tok(&tok); 
            return l;
        }
        Node assign_expr = make_node(ND_ASSIGNEXPR, PERM);
        assign_expr->loc = tok->loc;
        add_child(assign_expr, &l);
        Node r;
        if (tok->type != PUNCT) {
            // error expected assignment_operator
        } else {
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

                // error has to be one of the above

            }
            r = assignment_expression(); 
            add_child(assign_expr, &r);
            return assign_expr;

        }
    }
    return l;
}
Node constant_expression(){
    return conditional_expression();
} 

// declarations
Node declaration(){}
Node declaration_specifiers(){} 
Node init_declarator_list(){} 
Node static_assert_declaration(){
    // _Static_assert ( constant_expression , string-literal ) ;

    Node sad = make_node(ND_SAD, PERM);
    Token tok;
    
    if ((tok = lex())->type != KEYWORD || tok->subtype != _STATIC_ASSERT) {
        // error - expected _static_assert

        // error recovery, return appropriate node with err bit 
    } 
    sad->loc = tok->loc;

    if ((tok = lex())->type != PUNCT || tok->subtype != LBRAC) {
        // error - expected '('
    }
    
    // constant expression
    Node const_ex = constant_expression();
    add_child(sad, &const_ex);
    
    if ((tok = lex())->type != PUNCT || tok->subtype != COM) {
        // error - expected ','
    }
    
    // string literal 
    if ((tok = lex())->type != STR) {
        // error - expected a string constant
    }
    Node str = make_node(ND_STR, PERM);
    str->val.strval = tok->val.strval;
    str->loc = tok->loc;
    add_child(sad, &str);

    // ')'
    if ((tok = lex())->type != PUNCT || tok->subtype != RBRAC) {
        // error - expected ')'
    }
    if ((tok = lex())->type != PUNCT || tok->subtype != SCOL) {
        // error - expected ';'

    }
    return sad;
}
Node storage_class_specifier(){}
Node type_specifier(){} 
Node type_qualifier(){} 
Node function_specifier(){} 
Node alignment_specifier(){} 
Node init_declarator(){} 
Node declarator(){} 
Node atomic_type_specifier(){} 
Node struct_or_union_specifier(){} 
Node enum_specifier(){}
Node typedef_name(){} 
// Node struct_or_union(){} not needed; only contains terminals
Node struct_declaration_list(){} 
Node struct_declaration(){}
Node specifier_qualifier_list(){}
Node struct_declarator_list(){}
Node struct_declarator(){}
Node enumerator_list(){}
Node enumerator(){}
Node enumeration_constant(){}
Node pointer(){} 
Node direct_declarator(){} 
Node type_qualifier_list(){}
Node parameter_type_list(){} 
Node identifier_list(){} 
Node parameter_list(){} 
Node parameter_declaration(){} 
Node abstract_declarator(){}
Node direct_abstract_declarator(){}
Node initializer(){}
Node initializer_list(){}
Node designation(){} 
Node designator_list(){}

// statements
Node statement(){} 
Node labeled_statement(){}
Node compound_statement(){} 
Node expression_statement(){} 
Node selection_statement(){} 
Node iteration_statement(){} 
Node jump_statement(){} 
Node block_item_list(){} 
Node block_item(){} 

// external definitions 
Node translation_unit(){} 
Node external_declartion(){} 
Node function_definition(){} 
Node declaration_list(){}


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
    char idnt[indent + 1];
    memset(idnt, ' ', indent);
    idnt[indent] = '\0';
    printf("%s", idnt);
    switch (n->id) {
        case ND_ROOT:
            printf("-%s \n", node_map[ND_ROOT - 256]);
            break;
        case ND_ID: 
            printf("-%s \"%s\" <line:%d, col:%d>\n", node_map[n->id - 256], \
                    n->val.strval, n->loc.y, n->loc.x);
            break;
        case ND_STR:
            printf("-%s \"%s\" <line:%d, col:%d>\n", node_map[n->id - 256], \
                    n->val.strval, n->loc.y, n->loc.x);
            break;
        case ND_CONST:
            switch (n->subid) {
                case ND_UCHAR:
                    printf("-%s:%s \'%c\'<line:%d, col:%d>\n", node_map[n->id - 256], \
                            node_map[n->subid - 256], n->val.charval.c, n->loc.y, n->loc.x);
                    break; 
                case ND_INT: 
                    switch (n->subsubid) {
                        case ULLONG: 
                            printf("-%s:%s:ULLONG %llu <line:%d, col:%d>\n", node_map[ n->id - 256], \
                                   node_map[n->subid - 256], n->val.intval.ull, n->loc.y, n->loc.x);
                            break;
                        case SLLONG: 
                            printf("-%s:%s:SLLONG %lld <line:%d, col:%d>\n", node_map[n->id - 256], \
                                   node_map[n->subid - 256], n->val.intval.ll, n->loc.y, n->loc.x);
                            break;
                        case ULONG: 
                            printf("-%s:%s:ULONG %lu <line:%d, col:%d>\n", node_map[n->id - 256], \
                                   node_map[n->subid - 256], n->val.intval.ul, n->loc.y, n->loc.x);
                            break;
                        case SLONG: 
                            printf("-%s:%s:SLONG %ld <line:%d, col:%d>\n", node_map[n->id - 256], \
                                   node_map[n->subid - 256], n->val.intval.l, n->loc.y, n->loc.x);
                            break;
                        case UINT: 
                            printf("-%s:%s:UINT %u <line:%d, col:%d>\n", node_map[n->id - 256], \
                                   node_map[n->subid - 256], n->val.intval.u, n->loc.y, n->loc.x);
                            break;
                        case SINT: 
                            printf("-%s:%s:SINT %d <line:%d, col:%d>\n", node_map[n->id - 256], \
                                   node_map[n->subid - 256], n->val.intval.i, n->loc.y, n->loc.x);
                            break; 
                        }
                    break; 
                case ND_FLOAT:
                    switch (n->subsubid) {
                        case SDOUBLE: 
                            printf("-%s:%s:SDOUBLE %f <line:%d, col:%d>\n", node_map[n->id - 256], \
                                   node_map[n->subid - 256], n->val.floatval.d, n->loc.y, n->loc.x);
                            break;
                        case LDOUBLE: 
                            printf("-%s:%s:LDOUBLE %Lf <line:%d, col:%d>\n", node_map[n->id - 256], \
                                   node_map[n->subid - 256], n->val.floatval.ld, n->loc.y, n->loc.x);
                            break;
                        case SFLOAT: 
                            printf("-%s:%s:SFLOAT %f <line:%d, col:%d>\n", node_map[n->id - 256], \
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
            printf("-%s <line:%d, col:%d>\n", node_map[n->id - 256], \
                n->loc.y, n->loc.x);
            break;
        case ND_ASSIGNEXPR: 
            printf("-%s:%s <line:%d, col:%d>\n", node_map[n->id - 256], \
                    node_map[n->subid - 256], n->loc.y, n->loc.x);
            break;
        case ND_COND: 
            printf("-%s <line:%d, col:%d>\n", node_map[n->id - 256], \
                    n->loc.y, n->loc.x);
            break;
        case ND_OR: case ND_AND: case ND_IOR: case ND_XOR: 
        case ND_BAND: case ND_NEQ: case ND_EQ: case ND_GEQ:
        case ND_LEQ: case ND_GREATER: case ND_LESS: 
        case ND_LSHFT: case ND_RSHFT: case ND_SUB: case ND_ADD:
        case ND_MOD: case ND_DIV: case ND_MULT: 
            printf("-%s <line:%d, col:%d>\n", node_map[n->id - 256], \
                    n->loc.y, n->loc.x);
        default: 

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
        dump_AST(kid, indent + 4);
        kid = kid->nxt;
    }
}


