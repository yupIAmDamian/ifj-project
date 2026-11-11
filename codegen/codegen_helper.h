#ifndef CODEGEN_HELPER_H
#define CODEGEN_HELPER_H
#include "ast.h"

Token get_token_from_node(ASTNode *node);

char* string_from_token(ASTNode *node);

#endif