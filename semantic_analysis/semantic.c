#include <stdio.h>
#include <stdlib.h>
#include "semantic.h"

static void check_statement(ASTNode *node, Symtable *st);
//static DataType check_expression(ASTNode *node, Symtable *st)TODO

bool check_semantics(ASTNode *root, Symtable *st) {
    if (root == NULL) return true;
    check_statement(root, st);
    
    return true;
}

static void check_statement(ASTNode *node, Symtable *st) {
    if (node == NULL) return;
    switch (node->type) {
        case NODE_VAR_DECL: {
            DataType expr_type = check_expression(node->right, st);
            
            bool success = symtable_insert_var(st, node->val.string_val, node->inferred_type, true);
            if (!success) {
                fprintf(stderr, "Semantic Error: Redefinition of variable '%s'\n", node->val.string_val);
                exit(3);
            }
            if (node->inferred_type != expr_type) {
                fprintf(stderr, "Semantic Error: Cannot assign %d to variable of type %d\n", expr_type, node->inferred_type);
                exit(4);
            }
            break;
        }

        case NODE_IF: {
            DataType cond_type = check_expression(node->left, st);
            if (cond_type != TYPE_BOOL) {
                fprintf(stderr, "Semantic Error: Condition in 'if' must be Boolean\n");
                exit(4);
            }

            scope_push(st);
            check_statement(node->right, st);
            scope_pop(st);
            if (node->extra != NULL) {
                scope_push(st);
                check_statement(node->extra, st);
                scope_pop(st);
            }
            break;
        }
    }
    if (node->next != NULL) {
        check_statement(node->next, st);
    }
}