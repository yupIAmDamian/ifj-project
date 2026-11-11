#include "codegen_helper.h"

Token get_token_from_node(ASTNode *node) {
    Token token = node->val;
    return token;
}

char* string_from_token(ASTNode *node) {
    Token token = get_token_from_node(node);
    return token.attribute.string_value;
}