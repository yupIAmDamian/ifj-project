#include "codegen_tests.h"

static Token make_token(TokenType type, int line) {
    Token result = {0};
    result.type = type;
    result.line = line;
    return result;
}

static Token identifier_token(const char *name, int line) {
    Token result = make_token(TOKEN_IDENTIFIER, line);
    result.attribute.string_value = (char *)name;
    return result;
}

static Token integer_token(int value, int line) {
    Token result = make_token(TOKEN_INTEGER, line);
    result.attribute.integer_value = value;
    return result;
}

static Token string_token(const char *value, int line) {
    Token result = make_token(TOKEN_STRING, line);
    result.attribute.string_value = (char *)value;
    return result;
}

static ASTNode *term_identifier(const char *name, int line) {
    return create_ast_node(AST_NODE_TERM, NULL, NULL, NULL,
                           identifier_token(name, line));
}

static ASTNode *term_integer(int value, int line) {
    return create_ast_node(AST_NODE_TERM, NULL, NULL, NULL,
                           integer_token(value, line));
}

static ASTNode *term_string(const char *value, int line) {
    return create_ast_node(AST_NODE_TERM, NULL, NULL, NULL,
                           string_token(value, line));
}

static ASTNode *binary_expression(ASTNodeType type, TokenType operator_type,
                                  ASTNode *left, ASTNode *right, int line) {
    return create_ast_node(type, left, right, NULL,
                           make_token(operator_type, line));
}

static ASTNode *call(const char *name, ASTNode *arguments, int line) {
    return create_ast_node(AST_NODE_CALL, NULL, arguments, NULL,
                           identifier_token(name, line));
}

static ASTNode *variable(const char *name, int line) {
    return create_ast_node(AST_NODE_VAR, NULL, NULL, NULL,
                           identifier_token(name, line));
}

static ASTNode *declaration(const char *name, ASTNode *initializer, int line) {
    return create_ast_node(AST_NODE_VAR_DECL, variable(name, line),
                           initializer, NULL, identifier_token(name, line));
}

static ASTNode *assignment(const char *name, ASTNode *value, int line) {
    return create_ast_node(AST_NODE_ASSIGN, variable(name, line), value,
                           NULL, identifier_token(name, line));
}

static void append_node(ASTNode **first, ASTNode **last, ASTNode *node) {
    if (*first == NULL) {
        *first = node;
    } else {
        (*last)->next = node;
        node->prev = *last;
    }
    *last = node;
}

static ASTNode *create_factorial_tree(void) {
    ASTNode *main_body = NULL;
    ASTNode *main_last = NULL;

    append_node(&main_body, &main_last,
                declaration("n", call("readInt", NULL, 4), 4));

    ASTNode *calc_argument = term_integer(5, 5);
    ASTNode *calc_call = call("calc", calc_argument, 5);
    append_node(&main_body, &main_last,
                declaration("k",
                            binary_expression(AST_NODE_ADD, TOKEN_PLUS,
                                              term_integer(5, 5), calc_call, 5),
                            5));
    append_node(&main_body, &main_last,
                call("print", term_identifier("k", 6), 6));

    ASTNode *then_body = call(
        "print",
        term_string("Factorial is not defined for negative numbers\n", 8),
        8
    );

    ASTNode *else_body = NULL;
    ASTNode *else_last = NULL;
    append_node(&else_body, &else_last,
                declaration("f", term_integer(1, 10), 10));
    append_node(&else_body, &else_last,
                declaration("i", term_integer(1, 11), 11));

    ASTNode *while_body = NULL;
    ASTNode *while_last = NULL;
    append_node(&while_body, &while_last,
                assignment("f",
                           binary_expression(AST_NODE_MUL, TOKEN_MUL,
                                             term_identifier("f", 13),
                                             term_identifier("i", 13), 13),
                           13));
    append_node(&while_body, &while_last,
                assignment("i",
                           binary_expression(AST_NODE_ADD, TOKEN_PLUS,
                                             term_identifier("i", 14),
                                             term_integer(1, 14), 14),
                           14));

    ASTNode *while_node = create_ast_node(
        AST_NODE_WHILE,
        binary_expression(AST_NODE_EXPR, TOKEN_LESS_EQ,
                          term_identifier("i", 12),
                          term_identifier("n", 12), 12),
        while_body,
        NULL,
        make_token(TOKEN_WHILE, 12)
    );
    append_node(&else_body, &else_last, while_node);
    append_node(&else_body, &else_last,
                call("print", term_identifier("f", 15), 15));
    append_node(&else_body, &else_last,
                call("print", term_string("\n", 16), 16));

    append_node(&main_body, &main_last,
                create_ast_node(
                    AST_NODE_IF,
                    binary_expression(AST_NODE_EXPR, TOKEN_LESS,
                                      term_identifier("n", 7),
                                      term_integer(0, 7), 7),
                    then_body,
                    else_body,
                    make_token(TOKEN_IF, 7)
                ));

    ASTNode *main_function = create_ast_node(
        AST_NODE_FUNCTION,
        NULL,
        main_body,
        NULL,
        identifier_token("run", 3)
    );
    main_function->is_main = true;

    ASTNode *calc_body = create_ast_node(
        AST_NODE_RETURN,
        binary_expression(
            AST_NODE_ADD,
            TOKEN_PLUS,
            term_identifier("x", 20),
            binary_expression(AST_NODE_MUL, TOKEN_MUL,
                              term_integer(8, 20),
                              term_integer(7, 20), 20),
            20
        ),
        NULL,
        NULL,
        make_token(TOKEN_RETURN, 20)
    );

    ASTNode *calc_parameter = create_ast_node(
        AST_NODE_PARAM, NULL, NULL, NULL, identifier_token("x", 19));
    ASTNode *calc_function = create_ast_node(
        AST_NODE_FUNCTION,
        calc_parameter,
        calc_body,
        NULL,
        identifier_token("calc", 19)
    );

    main_function->next = calc_function;
    calc_function->prev = main_function;

    return create_ast_node(
        AST_NODE_PROGRAM,
        main_function,
        NULL,
        NULL,
        make_token(TOKEN_EOF, 21)
    );
}

void codegen_tests(void) {
    ASTNode *root = create_factorial_tree();
    gen_code(root);
}
