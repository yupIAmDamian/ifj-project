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

static ASTNode *expression_value(Token token) {
    return create_ast_node(AST_NODE_EXPR, NULL, NULL, NULL, token);
}

static ASTNode *identifier_expression(const char *name, int line) {
    return expression_value(identifier_token(name, line));
}

static ASTNode *integer_expression(int value, int line) {
    return expression_value(integer_token(value, line));
}

static ASTNode *term(Token token) {
    return create_ast_node(AST_NODE_TERM, NULL, NULL, NULL, token);
}

static ASTNode *term_identifier(const char *name, int line) {
    return term(identifier_token(name, line));
}

static ASTNode *term_string(const char *value, int line) {
    return term(string_token(value, line));
}

static ASTNode *binary_expression(TokenType operator_type,
                                  ASTNode *left, ASTNode *right, int line) {
    return create_ast_node(AST_NODE_EXPR, left, right, NULL,
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

static ASTNode *declaration(const char *name, ASTNode *initializer,
                            bool is_const, int line) {
    ASTNode *var_node = variable(name, line);
    ASTNode *node = create_ast_node(AST_NODE_VAR_DECL, var_node, initializer,
                                    NULL, identifier_token(name, line));
    node->is_const = is_const;
    var_node->is_const = is_const;
    return node;
}

static ASTNode *assignment(const char *name, ASTNode *value, int line) {
    return create_ast_node(AST_NODE_ASSIGN, variable(name, line), value,
                           NULL, identifier_token(name, line));
}

static ASTNode *return_statement(ASTNode *value, int line) {
    return create_ast_node(AST_NODE_RETURN, value, NULL, NULL,
                           make_token(TOKEN_RETURN, line));
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

static ASTNode *parameter(const char *name, int line) {
    return create_ast_node(AST_NODE_PARAM, NULL, NULL, NULL,
                           identifier_token(name, line));
}

static ASTNode *if_node(ASTNode *condition, ASTNode *then_body,
                        ASTNode *else_body, int line) {
    return create_ast_node(AST_NODE_IF, condition, then_body, else_body,
                           make_token(TOKEN_IF, line));
}

static ASTNode *while_node(ASTNode *condition, ASTNode *body, int line) {
    return create_ast_node(AST_NODE_WHILE, condition, body, NULL,
                           make_token(TOKEN_WHILE, line));
}

static ASTNode *function(const char *name, ASTNode *parameters,
                         ASTNode *body, bool is_main, int line) {
    ASTNode *node = create_ast_node(AST_NODE_FUNCTION, parameters, body, NULL,
                                    identifier_token(name, line));
    node->is_main = is_main;
    return node;
}

static ASTNode *create_factorial_tree(void) {
    ASTNode *iterative_body = NULL;
    ASTNode *iterative_last = NULL;

    append_node(&iterative_body, &iterative_last,
                declaration("result", integer_expression(1, 4), false, 4));
    append_node(&iterative_body, &iterative_last,
                declaration("i", integer_expression(2, 5), false, 5));

    ASTNode *iterative_loop_body = NULL;
    ASTNode *iterative_loop_last = NULL;
    append_node(&iterative_loop_body, &iterative_loop_last,
                assignment("result",
                           binary_expression(
                               TOKEN_MUL,
                               identifier_expression("result", 7),
                               identifier_expression("i", 7), 7),
                           7));
    append_node(&iterative_loop_body, &iterative_loop_last,
                assignment("i",
                           binary_expression(
                               TOKEN_PLUS,
                               identifier_expression("i", 8),
                               integer_expression(1, 8), 8),
                           8));
    append_node(&iterative_body, &iterative_last,
                while_node(
                    binary_expression(TOKEN_LESS_EQ,
                                      identifier_expression("i", 6),
                                      identifier_expression("n", 6), 6),
                    iterative_loop_body, 6));
    append_node(&iterative_body, &iterative_last,
                identifier_expression("result", 9));

    ASTNode *iterative_function = function(
        "factorialIterative", parameter("n", 3), iterative_body, false, 3);

    ASTNode *recursive_then = return_statement(integer_expression(1, 13), 13);
    ASTNode *recursive_else = NULL;
    ASTNode *recursive_else_last = NULL;
    append_node(&recursive_else, &recursive_else_last,
                declaration("previous",
                            binary_expression(TOKEN_MINUS,
                                              identifier_expression("n", 15),
                                              integer_expression(1, 15), 15),
                            true, 15));
    append_node(&recursive_else, &recursive_else_last,
                declaration("smaller",
                            call("factorialRecursive",
                                 term_identifier("previous", 16), 16),
                            true, 16));
    append_node(&recursive_else, &recursive_else_last,
                return_statement(
                    binary_expression(TOKEN_MUL,
                                      identifier_expression("n", 17),
                                      identifier_expression("smaller", 17), 17),
                    17));

    ASTNode *recursive_body = if_node(
        binary_expression(TOKEN_LESS_EQ,
                          identifier_expression("n", 12),
                          integer_expression(1, 12), 12),
        recursive_then, recursive_else, 12);
    ASTNode *recursive_function = function(
        "factorialRecursive", parameter("n", 11), recursive_body, false, 11);

    ASTNode *main_body = NULL;
    ASTNode *main_last = NULL;
    append_node(&main_body, &main_last,
                declaration("n", call("readInt", NULL, 20), true, 20));

    ASTNode *negative_then = call(
        "print",
        term_string("Use an integer from 0 to 12.\n", 25), 25);
    ASTNode *too_large_then = call(
        "print",
        term_string("Use an integer from 0 to 12.\n", 28), 28);

    ASTNode *valid_else = NULL;
    ASTNode *valid_else_last = NULL;
    append_node(&valid_else, &valid_else_last,
                declaration("iterative",
                            call("factorialIterative",
                                 term_identifier("n", 30), 30),
                            true, 30));
    append_node(&valid_else, &valid_else_last,
                declaration("recursive",
                            call("factorialRecursive",
                                 term_identifier("n", 31), 31),
                            true, 31));
    append_node(&valid_else, &valid_else_last,
                call("print", term_string("iterative: ", 32), 32));
    append_node(&valid_else, &valid_else_last,
                call("print", term_identifier("iterative", 33), 33));
    append_node(&valid_else, &valid_else_last,
                call("print", term_string("\nrecursive: ", 34), 34));
    append_node(&valid_else, &valid_else_last,
                call("print", term_identifier("recursive", 35), 35));
    append_node(&valid_else, &valid_else_last,
                call("print", term_string("\n", 36), 36));

    ASTNode *range_else = if_node(
        binary_expression(TOKEN_GREATER,
                          identifier_expression("n", 27),
                          integer_expression(12, 27), 27),
        too_large_then, valid_else, 27);
    append_node(&main_body, &main_last,
                if_node(
                    binary_expression(TOKEN_LESS,
                                      identifier_expression("n", 24),
                                      integer_expression(0, 24), 24),
                    negative_then, range_else, 24));

    ASTNode *main_function = function(
        "factorialExample", NULL, main_body, true, 19);
    iterative_function->next = recursive_function;
    recursive_function->prev = iterative_function;
    recursive_function->next = main_function;
    main_function->prev = recursive_function;

    return create_ast_node(AST_NODE_PROGRAM, iterative_function, NULL, NULL,
                           make_token(TOKEN_EOF, 36));
}

void codegen_tests(void) {
    ASTNode *root = create_factorial_tree();
    gen_code(root);
}


