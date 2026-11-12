#ifndef CODEGEN_TESTS_H
#define CODEGEN_TESTS_H
#include "../../codegen/codegen.h"
#include "../../syntactic_analysis/ast.h"
#include "../../scanner/scanner.h"

Token token(TokenType type, int line);
Token identifier_token(const char *name, int line);
Token integer_token(int value, int line);
Token string_token(const char *value, int line);

ASTNode *identifier(const char *name, int line);
ASTNode *term_identifier(const char *name, int line);
ASTNode *term_integer(int value, int line);
ASTNode *term_string(const char *value, int line);
ASTNode *expression(ASTNodeType type, Token operator_token,
                           ASTNode *left, ASTNode *right);
ASTNode *call(const char *name, ASTNode *arguments, int line);
ASTNode *declaration(const char *name, ASTNode *initializer, int line);
ASTNode *assignment(const char *name, ASTNode *value, int line);
void append_node(ASTNode **first, ASTNode **last, ASTNode *node);

ASTNode *create_tree(void);

void codegen_tests();

#endif