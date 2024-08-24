buffer buf;
location loc;

Node root;
token_store *ts;

// storage class specifier constants
#define SCS_TYPEDEF         0x1
#define SCS_EXTERN          0x2
#define SCS_STATIC          0x4 
#define SCS_THREAD_LOCAL    0x8
#define SCS_AUTO            0x10
#define SCS_REGISTER        0x20
// type specifier constants
#define TS_VOID             0x1
#define TS_CHAR             0x2
#define TS_SHORT            0x4
#define TS_INT              0x8
#define TS_LONG             0x10
#define TS_LLONG            0x20
#define TS_FLOAT            0x40
#define TS_DOUBLE           0x80
#define TS_SIGNED           0x100
#define TS_UNSIGNED         0x200
#define TS_BOOL             0x400
#define TS_COMPLEX          0x400
#define TS_ATOMIC           0x1000
#define TS_STRUCT           0x2000
#define TS_UNION            0x4000
#define TS_ENUM             0x8000
// type qualifier constants
#define TQ_CONST            0x1
#define TQ_RESTRICT         0x2
#define TQ_VOLATILE         0x4
#define TQ_ATOMIC           0x8
// function specifier constants
#define FS_INLINE           0x1
#define FS_NORETURN         0x2

struct dec_spec {
    // storage class specifier
    unsigned int scs:6;
    // type specifier
    unsigned int ts:16;
    // type qualifier
    unsigned int tq:3;
    // function specifier
    unsigned int fs:2;
    // typedef name: TODO
};


// parsing functions

// expressions
Node primary_expression();
Node generic_selection();
Node generic_assoc_list();
Node generic_association();
Node postfix_expression();
Node argument_expression_list();
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
Node enumeration_constant();

// declarations.c
Type declaration_specifiers();
int storage_class_specifier(struct dec_spec *ds);
int type_specifier(struct dec_spec *ds, Type *t);
Type struct_or_union_specifier();
Field struct_declaration_list();
Node struct_declaration();
Node specifier_qualifier_list();
Node struct_declarator_list();
Node struct_declarator();
Node enum_specifier();
Node enumerator_list();
Node enumerator();
Node atomic_type_specifier();
int type_qualifier(struct dec_spec *ds);
int function_specifier(struct dec_spec *ds);
int alignment_specifier(struct dec_spec *ds);
Node init_declarator_list(Type ds);
Node init_declarator(Type ds);
Node declarator(Type ds);
Node direct_declarator(Type ds);
Type pointer(Type ds);
Type type_qualifier_list(Type ds);
Node parameter_type_list();
Node parameter_list();
Node parameter_declaration();
Node identifier_list();
Node type_name();
Node abstract_declarator();
Node direct_abstract_declarator();
Node initializer();
Node initializer_list();
Node designation();
Node designator_list();
Node designator();
Node static_assert_declaration();
Type build_type(struct dec_spec ds, Type ty);

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
_Bool first(int id, Token tok);
//Type build_type(struct dec_spec ds, Type tt);
