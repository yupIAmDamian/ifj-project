// Compiler of IFJ26 imperative language
// Author: Michal Kopček (xkopcem00)

#include <stdlib.h>
#include "ast.h"

ASTNode *create_ast_node(ASTNodeType type, ASTNode *left, ASTNode *right, ASTNode *extra, Token val) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (node == NULL) {
        return NULL;
    }
    node->type = type;
    node->left = left;
    node->right = right;
    node->extra = extra;
    node->val = val;
    node->next = NULL;
    return node;
}