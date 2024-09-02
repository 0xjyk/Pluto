#include "pluto.h"
#include "include/parse.h"

// statements
Node statement(){
    // labeled_statement
    // compound_statement
    // expression_statement
    // selection_statement
    // iteration_statement
    // jump_statement

    Token tok = lex(); 
    // need two token look ahead identifiers
    if (tok->type == ID) {
        Token tokcpy = tok;
        tok = lex(); 
        // labeled statement
        if (tok->subtype == COL) {
            // restore in opposite order of retrieval
            restore_tok(&tok);
            restore_tok(&tokcpy);
            return labeled_statement();
        }
        // expression statement
        restore_tok(&tok);
        restore_tok(&tokcpy);
        return expression_statement();
        // expression statement
    } else if ((first(ND_EXPR, tok) && tok->subtype != LCBRAC) || tok->subtype == SCOL) {
        restore_tok(&tok);
        return expression_statement();
    }
    switch(tok->subtype) {
        case CASE: case DEFAULT: 
            restore_tok(&tok); return labeled_statement();
        case LCBRAC:
            restore_tok(&tok); return compound_statement();
        case IF: case SWITCH:
            restore_tok(&tok); return selection_statement();
        case WHILE: case DO: case FOR: 
            restore_tok(&tok); return iteration_statement();
        case GOTO: case CONTINUE: 
        case BREAK: case RETURN:
            restore_tok(&tok); return jump_statement();
        default:
            error(&tok->loc, "encountered unknown statement");
            // escape till FOLLOW(statement)
            break;
    }
    // return dummy statement
}
Node labeled_statement(){
    // identifier : statement
    // case constant-expression : statement
    // default : statement
    Token tok = lex();
    Node label_stmt = make_node(0, PERM);
    label_stmt->loc = tok->loc;
    switch(tok->subtype) {
        case ID: 
            label_stmt->id = ND_ID_LABEL;
            Symbol s = lookup(tok->val.strval, labels);
            if (s && s->defined) {
                error(&tok->loc, "attempting to redefine previously defined label");
            }
            label_stmt->sym = install(tok->val.strval, &labels, level, FUNC);
            label_stmt->sym->defined = 1;
            label_stmt->sym->loc = tok->loc;
            break;
        // both case and default cannot contains VLAs todo
        case CASE:
            if (!tc.in_switch) {
                error(&tok->loc, "attempting to declare case statement outside switch statement");
            }
            label_stmt->id = ND_CASE_LABEL;
            Node const_expr = constant_expression();
            add_child(label_stmt, &const_expr);
            break;
        case DEFAULT:
            if (!tc.in_switch) {
                error(&tok->loc, "attempting to declare default statement outside switch statement");
            }
            label_stmt->id = ND_DEFAULT_LABEL;
        // default case not needed
    }
    if ((tok = lex())->subtype != COL) {
        error(&tok->loc, "expected ':' between case expression and statement");
        restore_tok(&tok);
    }
    Node stmt = statement();
    add_child(label_stmt, &stmt);
    return label_stmt;
}
Node compound_statement(){
    // { block_item_list[opt] }
    Token tok = lex();
    Node compd_stmt = make_node(ND_COMPOUND, PERM);
    compd_stmt->loc = tok->loc;
    if (tok->subtype != LCBRAC) {
        error(&tok->loc, "expected opening '{' before compound expression");
        restore_tok(&tok);
    }
    Node block_item;
    // block_item 
    // block_item_list block_item
    while ((tok = lex())->subtype != RCBRAC) {
        // declaration
        // statement 
        if (first(ND_DS, tok)) {
            restore_tok(&tok);
            block_item = declaration();
            add_child(compd_stmt, &block_item);
            continue;
        }
        restore_tok(&tok);
        block_item = statement();
        add_child(compd_stmt, &block_item);
        continue;
    }
    return compd_stmt;
}


Node expression_statement(){
    // expression[opt] ;
    Token tok = lex();
    Node expr;
    if (tok->subtype == SCOL) {
        expr = make_node(ND_EXPR, PERM);
        expr->loc = tok->loc;
        return expr;
    } 
    restore_tok(&tok);
    expr = expression();
    if ((tok = lex())->subtype != SCOL) {
        restore_tok(&tok);
        error(&tok->loc, "exptected ';' after expression statement");
    }
    return expr; 
}
Node selection_statement(){
    // if ( expression ) statement 
    // if ( expression ) statement else statement
    // switch ( expression ) statement
    Token tok = lex();
    Node sel_stmt = make_node(0, PERM);
    sel_stmt->loc = tok->loc;
    if (tok->subtype == IF)
        sel_stmt->id = ND_IF_STMT;
    else if (tok->subtype == SWITCH) {
        tc.in_switch++;
        sel_stmt->id = ND_SWITCH_STMT;
    } else
        sel_stmt->id = ND_IF_STMT;
    // ( expression ) statement
    if ((tok = lex())->subtype != LBRAC) {
        restore_tok(&tok);
        error(&tok->loc, "expected '(' after if/switch keyword");
    }
    Node expr = expression();
    // type check and ensure that 
    // if ND_IF_STMT, then the resulting type is a scalar type todo
    // if ND_STMT, then the resulting type is of integer type todo
    add_child(sel_stmt, &expr);
    if ((tok = lex())->subtype != RBRAC) {
        restore_tok(&tok);
        error(&tok->loc, "expected ')' after predicate expression in if/switch statement");
    }
    Node stmt = statement();
    add_child(sel_stmt, &stmt);
    if (sel_stmt->id == ND_SWITCH_STMT)
        tc.in_switch--;
    // optional else extension
    if (sel_stmt->id == ND_IF_STMT) 
        if (((tok = lex())->subtype == ELSE)) {
            stmt = statement();
            add_child(sel_stmt, &stmt);
        } else 
            restore_tok(&tok);
    return sel_stmt;
}
Node iteration_statement(){
    // while ( expression ) statement
    // do statement while ( expression ) ;
    // for ( expression[opt] ; expression[opt] ; expression[opt] ) statement
    // for ( declaration expression[opt] ; expression[opt] ) statement
    Token tok = lex();
    Node iter_stmt = make_node(0, PERM);
    iter_stmt->loc = tok->loc;
    Node expr, stmt;
    switch(tok->subtype) {
        case WHILE:
            enterscope();
            iter_stmt->id = ND_WHILE;
            if ((tok = lex())->subtype != LBRAC) {
                restore_tok(&tok);
                error(&tok->loc, "expetected '(' after while keyword");
            }
            expr = expression();
            add_child(iter_stmt, &expr);
            if ((tok = lex())->subtype != RBRAC) {
                restore_tok(&tok);
                error(&tok->loc, "expetected ')' after predicate expression in while statement");
            }
            enterscope();
            stmt = statement();
            exitscope();
            add_child(iter_stmt, &stmt);
            exitscope();
            break;
        case DO:
            enterscope();
            iter_stmt->id = ND_DO_WHILE;
            enterscope();
            stmt = statement();
            add_child(iter_stmt, &stmt);
            exitscope();
            if ((tok = lex())->subtype != WHILE) {
                restore_tok(&tok);
                error(&tok->loc, "expected 'while' keyword after 'do' statement");
            }
            if ((tok = lex())->subtype != LBRAC) {
                restore_tok(&tok);
                error(&tok->loc, "expected '(' after while keyword");
            }
            expr = expression();
            add_child(iter_stmt, &expr);
            if ((tok = lex())->subtype != RBRAC) {
                restore_tok(&tok);
                error(&tok->loc, "expected ')' after predicate expression in while statement");
            }
            if ((tok = lex())->subtype != SCOL) {
                restore_tok(&tok);
                error(&tok->loc, "expected ';' after while ( expression )");
            }
            exitscope();
            break;
        case FOR:
            enterscope();
            iter_stmt->id = ND_FOR;
            if ((tok = lex())->subtype != LBRAC) {
                restore_tok(&tok);
                error(&tok->loc, "expected '(' after for keyword");
            }
            tok = lex();
            if (first(ND_DS, tok)) {
                restore_tok(&tok);
                Node decl = declaration(); 
                add_child(iter_stmt, &decl);
            } else {
                restore_tok(&tok);
                expr = expression_statement(); 
                add_child(iter_stmt, &expr);
            }
            expr = expression_statement(); 
            add_child(iter_stmt, &expr);
            if ((tok = lex())->subtype != RBRAC) {
                restore_tok(&tok);
                expr = expression();
                add_child(iter_stmt, &expr);
                if ((tok = lex())->subtype != RBRAC) {
                    restore_tok(&tok);
                    error(&tok->loc, "expected ')' after predicate expression in for statement");
                }
            } else {
                expr = make_node(ND_EXPR, PERM);
                expr->loc = tok->loc;
                add_child(iter_stmt, &expr);
            }
                        
            enterscope();
            stmt = statement();
            exitscope();
            add_child(iter_stmt, &stmt);
            exitscope();
            break;
    }
    return iter_stmt;
}
Node jump_statement(){
    // goto identifier ;
    // continue ;
    // break ;
    // return expression[opt] ;
    Token tok = lex(); 
    Node jmp_stmt = make_node(0, PERM);
    jmp_stmt->loc = tok->loc;
    switch (tok->subtype) {
        case GOTO:
            jmp_stmt->id = ND_GOTO;
            if ((tok = lex())->type == ID) {
                Symbol sym = lookup(tok->val.strval, labels);
                if (!sym) {
                    jmp_stmt->sym = install(tok->val.strval, &labels, level, FUNC);
                    jmp_stmt->sym->defined = 0;
                } else 
                    jmp_stmt->sym = sym;
            } else {
                restore_tok(&tok);
                jmp_stmt->id = ND_CONTINUE;
            }
            break;
        case CONTINUE:
            jmp_stmt->id = ND_CONTINUE;
            break;
        case BREAK:
            jmp_stmt->id = ND_BREAK;
            break;
        case RETURN:
            jmp_stmt->id = ND_RETURN;
            Node expr = expression_statement();
            add_child(jmp_stmt, &expr);
            return jmp_stmt;
    }
    if ((tok = lex())->subtype != SCOL) {
        restore_tok(&tok);
        error(&tok->loc, "expected ';' after jump statement");
    }
    return jmp_stmt;
}

// external definitions
Node function_definition(Symbol sym){
    // declaration_specifiers declarator compound_statement 
    // *grammar has been slightly modified to disallow K&R style definitions
    Node func_def = make_node(ND_FUNC_DEF, PERM);
    Token tok = lex();
    location l = tok->loc;
    restore_tok(&tok);
    if (!sym) {
        error(&l, "interal error while attempting to define function");
        sym = alloc(sizeof(symbol), PERM);
        sym->type = make_func(voidtype, vec_init(2));
        vec_pushback(sym->type->u.f.proto, voidtype);
        vec_pushback(sym->type->u.f.proto, NULL);
        func_def->sym = sym;
        func_def->loc = l;
        func_def->type = sym->type;
        return func_def;
    }
    // overwrite type with default function type
    if (!isfunc(sym->type)) {
        error(&sym->loc, "attempting to define a non functional type as a function");
        sym->type = make_func(voidtype, vec_init(2));
        vec_pushback(sym->type->u.f.proto, voidtype);
        vec_pushback(sym->type->u.f.proto, NULL);   
    }
    // first lookup sym->name to see that it hasn't been previously defined
    Symbol s = lookup(sym->name, identifiers);
    // if s exits but with a different type, throw error
    if (!s) {
        s = install(sym->name, &identifiers, GLOBAL, PERM);
        s->type = sym->type;
        s->defined = 1;
        s->sclass = sym->sclass;
        s->loc = sym->loc;
    } else if (s && s->defined) {
        error(&sym->loc, "attempting to redefine previously defined function");
    } else if (s && !s->defined) {
        s->defined = 1;
    }
    // at this point s should be the correct symbol to define 
    func_def->sym = s;
    func_def->loc = l;
    func_def->type = s->type;
    func_def->val.strval = s->name;
    enterscope();
    // add each parameter to the newly created scope
    Vector proto = func_def->sym->type->u.f.proto;
    for (int i = 0; i < proto->size - 1; i++) {
        Symbol param = (Symbol) vec_get(proto, i);
        if (!param) {
            error(&l, "internal error: found null parameter in function definition");
            continue;
        }
        Symbol p = lookup(param->name, identifiers);
        if (p && p->scope == level) {
            error(&tok->loc, "attempting to declare parameters with duplicate identifiers");
        }
        p = install(param->name, &identifiers, level, FUNC);
        p->type = param->type;
        p->loc = param->loc;
    }
    Node comp_expr = compound_statement();
    exitscope();
    add_child(func_def, &comp_expr);
    dealloc(FUNC);
    return func_def;
}

