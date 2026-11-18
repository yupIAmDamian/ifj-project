// Compiler of IFJ26 imperative language
// Author: Michal Kopček (xkopcem00)

#include "ast.h"

static ASTNode *all_nodes = NULL;

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
    node->alloc_next = all_nodes;
    all_nodes = node;
    node->inferred_type = TYPE_UNKNOWN;
    return node;
}

void ast_cleanup(void) {
    while (all_nodes != NULL) {
        ASTNode *next = all_nodes->alloc_next;
        if ((all_nodes->val.type == TOKEN_IDENTIFIER || all_nodes->val.type == TOKEN_STRING) &&
            all_nodes->val.attribute.string_value != NULL) {
            free(all_nodes->val.attribute.string_value);
        }
        free(all_nodes);
        all_nodes = next;
    }
}