buffer buf;
location loc;

Node root;
token_store *ts;


// parsing functions

// expressions
Node primary_expression();
Node generic_selection();
Node generic_assoc_list();
Node generic_association();
Node type_name();
Node postfix_expression();
Node argument_expression_list();
Node initializer_list();
Node unary_expression();
Node cast_expression();
Node multiplicative_expression();
Node additive_expression();
Node shift_expression();
Node relational_expression();
Node equality_expression();
Node AND_expression();
Node exclusive_OR_expression();
Node inclusive_OR_expression();
Node logical_AND_expression();
Node logical_OR_expression();
Node conditional_expression();
Node assignment_expression();
Node expression();
Node constant_expression();

// declarations
Node declaration();
Node declaration_specifiers();
Node init_declarator_list();
Node static_assert_declaration();
Node storage_class_specifier();
Node type_specifier();
Node type_qualifier();
Node function_specifier();
Node alignment_specifier();
Node init_declarator();
Node declarator();
Node initializer();
Node atomic_type_specifier();
Node struct_or_union_specifier();
Node enum_specifier();
Node typedef_name();
// Node struct_or_union(); not needed; only contains terminals
Node struct_declaration_list();
Node struct_declaration();
Node specifier_qualifier_list();
Node struct_declarator_list();
Node struct_declarator();
Node enumerator_list();
Node enumerator();
Node enumeration_constant();
Node pointer();
Node direct_declarator();
Node type_qualifier_list();
Node parameter_type_list();
Node identifier_list();
Node parameter_list();
Node parameter_declaration();
Node abstract_declarator();
Node direct_abstract_declarator();
Node designation();
Node designator_list();

// statements
Node statement();
Node labeled_statement();
Node compound_statement();
Node expression_statement();
Node selection_statement();
Node iteration_statement();
Node jump_statement();
Node block_item_list();
Node block_item();

// external definitions
Node translation_unit(); // root of the AST
Node external_declartion();
Node function_definition();
Node declaration_list();

// helper functions
void print_cpp();
Node make_node(int id, int arena);
void add_child(Node n, Node *nw);
void restore_tok(Token *tok);
void print_node(Node n, int indent);
Node make_error_recovery_node();
void escape_first(int till);
