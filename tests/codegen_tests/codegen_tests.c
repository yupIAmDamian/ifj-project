#include "codegen_tests.h"

Token token(TokenType type, int line) {
    Token result = {0};
    result.type = type;
    result.line = line;
    return result;
}

Token identifier_token(const char *name, int line) {
    Token result = token(TOKEN_IDENTIFIER, line);
    result.attribute.string_value = (char *)name;
    return result;
}

Token integer_token(int value, int line) {
    Token result = token(TOKEN_INTEGER, line);
    result.attribute.integer_value = value;
    return result;
}

Token string_token(const char *value, int line) {
    Token result = token(TOKEN_STRING, line);
    result.attribute.string_value = (char *)value;
    return result;
}

ASTNode *identifier(const char *name, int line) {
    return create_ast_node(AST_NODE_VAR, NULL, NULL, NULL,
                           identifier_token(name, line));
}

ASTNode *term_identifier(const char *name, int line) {
    return create_ast_node(AST_NODE_TERM, NULL, NULL, NULL,
                           identifier_token(name, line));
}

ASTNode *term_integer(int value, int line) {
    return create_ast_node(AST_NODE_TERM, NULL, NULL, NULL,
                           integer_token(value, line));
}

ASTNode *term_string(const char *value, int line) {
    return create_ast_node(AST_NODE_TERM, NULL, NULL, NULL,
                           string_token(value, line));
}

ASTNode *expression(ASTNodeType type, Token operator_token,
                           ASTNode *left, ASTNode *right) {
    return create_ast_node(type, left, right, NULL, operator_token);
}

ASTNode *call(const char *name, ASTNode *arguments, int line) {
    return create_ast_node(AST_NODE_CALL, NULL, arguments, NULL,
                           identifier_token(name, line));
}

ASTNode *declaration(const char *name, ASTNode *initializer, int line) {
    return create_ast_node(AST_NODE_VAR_DECL, identifier(name, line),
                           initializer, NULL, identifier_token(name, line));
}

ASTNode *assignment(const char *name, ASTNode *value, int line) {
    return create_ast_node(AST_NODE_ASSIGN, identifier(name, line), value,
                           NULL, identifier_token(name, line));
}

void append_node(ASTNode **first, ASTNode **last, ASTNode *node) {
    if (*first == NULL) {
        *first = node;
    } else {
        (*last)->next = node;
        node->prev = *last;
    }
    *last = node;
}

ASTNode *create_tree(void) {
    ASTNode *body = NULL;
    ASTNode *body_last = NULL;

    append_node(&body, &body_last, declaration("n", term_integer(6, 4), 4));

    ASTNode *then_body = call(
        "print",
        term_string("Factorial is not defined for negative numbers\n", 6),
        6
    );

    ASTNode *else_body = NULL;
    ASTNode *else_last = NULL;
    append_node(&else_body, &else_last, declaration("f", term_integer(1, 8), 8));
    append_node(&else_body, &else_last, declaration("i", term_integer(1, 9), 9));

    ASTNode *while_body = NULL;
    ASTNode *while_last = NULL;
    append_node(
        &while_body,
        &while_last,
        assignment(
            "f",
            expression(
                AST_NODE_MUL,
                token(TOKEN_MUL, 11),
                term_identifier("f", 11),
                term_identifier("i", 11)
            ),
            11
        )
    );
    append_node(
        &while_body,
        &while_last,
        assignment(
            "i",
            expression(
                AST_NODE_ADD,
                token(TOKEN_PLUS, 12),
                term_identifier("i", 12),
                term_integer(1, 12)
            ),
            12
        )
    );

    ASTNode *while_node = create_ast_node(
        AST_NODE_WHILE,
        expression(
            AST_NODE_EXPR,
            token(TOKEN_LESS_EQ, 10),
            term_identifier("i", 10),
            term_identifier("n", 10)
        ),
        while_body,
        NULL,
        token(TOKEN_WHILE, 10)
    );
    append_node(&else_body, &else_last, while_node);
    append_node(&else_body, &else_last, call("print", term_identifier("f", 13), 13));
    append_node(&else_body, &else_last, call("print", term_string("\n", 14), 14));

    append_node(
        &body,
        &body_last,
        create_ast_node(
            AST_NODE_IF,
            expression(
                AST_NODE_EXPR,
                token(TOKEN_LESS, 5),
                term_identifier("n", 5),
                term_integer(0, 5)
            ),
            then_body,
            else_body,
            token(TOKEN_IF, 5)
        )
    );

    ASTNode *function = create_ast_node(
        AST_NODE_FUNCTION,
        NULL,
        body,
        NULL,
        identifier_token("run", 3)
    );
    function->is_main = true;

    return create_ast_node(AST_NODE_PROGRAM, function, NULL, NULL,
                           token(TOKEN_EOF, 15));
}

void codegen_tests() {
    ASTNode *root = create_tree();
    gen_code(root);
}