// Compiler of IFJ26 imperative language
// Author: Michal Kopček (xkopcem00)

#include "ast.h"

ASTNode *create_ast_node(ASTNodeType type, ASTNode *left, ASTNode *right, ASTNode *extra, Token val) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (node == NULL) {
        fprintf(stderr, "Internal error: Memory allocation failed in create_ast_node.\n");
        exit(99);
    }
    node->type = type;
    node->left = left;
    node->right = right;
    node->extra = extra;
    node->val = val;
    node->next = NULL;
    node->prev = NULL;
    node->is_main = false;
    node->is_const = false;
    return node;
}