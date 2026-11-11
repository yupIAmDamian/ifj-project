// Compiler of ifj 2026 iperative language
// Author: Daniel Jurkovič (xjurkod00)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "semantic.h"
// #include "error.h" dominiks error handling

static DataType current_func_ret_type = TYPE_VOID;

// functions for retype
// conversion node allocation (Int -> Float)
static ASTNode* wrap_int2float(ASTNode *expr) {
    if (expr == NULL) return NULL;

    ASTNode *conv = malloc(sizeof(ASTNode));
    if (conv == NULL) {
        dominik_error(99, "Internal Error: Memory allocation failed");
    }

    conv->type = NODE_INT2FLOAT;
    conv->inferred_type = TYPE_FLOAT;
    conv->left = expr;
    conv->right = NULL;
    conv->extra = NULL;
    conv->next = NULL;

    return conv;
}

// func check if retyping is needed and if so calls func to do it
static bool convert_if_needed(ASTNode **expr_ptr, DataType actual_type, DataType target_type) {
    if (actual_type == target_type) {
        return true;
    }
    if (target_type == TYPE_FLOAT && actual_type == TYPE_INT) {
        *expr_ptr = wrap_int2float(*expr_ptr);
        return true;
    }
    return false;
}


// MAIN SEMANTIC CONTROL
static void check_statement(ASTNode *node, Symtable *st);
static DataType check_expression(ASTNode *node, Symtable *st);

// initiates the recursive check on the root of the AST.
bool check_semantics(ASTNode *root, Symtable *st) {
    if (root == NULL) return true;
    check_statement(root, st);
    return true;
}

//recursively checks statement nodes and manages scope stack operations
static void check_statement(ASTNode *node, Symtable *st) {
    if (node == NULL) return;

    switch (node->type) {
        case NODE_VAR_DECL: {
            DataType expr_type = check_expression(node->right, st);
            
            bool success = symtable_insert_var(st, node->val.string_val, node->inferred_type, true);
            if (!success) {
                dominik_error(3, "Redefinition of variable");
            }

            if (!convert_if_needed(&node->right, expr_type, node->inferred_type)) {
                dominik_error(4, "Cannot assign incompatible type to variable");
            }
            break;
        }

        case NODE_ASSIGN: {
            Symbol *sym = symtable_search(st, node->left->val.string_val);
            if (sym == NULL) {
                dominik_error(3, "Assignment to undefined variable");
            }

            DataType expr_type = check_expression(node->right, st);
            if (!convert_if_needed(&node->right, expr_type, sym->data_type)) {
                dominik_error(4, "Type mismatch in assignment");
            }
            break;
        }

        case NODE_IF: {
            DataType cond_type = check_expression(node->left, st);
            if (cond_type != TYPE_BOOL) {
                dominik_error(4, "Condition in 'if' must be Boolean");
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

        case NODE_WHILE: {
            DataType cond_type = check_expression(node->left, st);
            if (cond_type != TYPE_BOOL) {
                dominik_error(4, "Condition in 'while' must be Boolean");
            }

            scope_push(st);
            check_statement(node->right, st);
            scope_pop(st);
            break;
        }

        case NODE_RETURN: {
            DataType return_type = TYPE_VOID;
            if (node->left != NULL) {
                return_type = check_expression(node->left, st);
            }

            if (!convert_if_needed(&node->left, return_type, current_func_ret_type)) {
                dominik_error(4, "Return type mismatch");
            }
            break;
        }

        case NODE_CALL: {
            check_expression(node, st);
            break;
        }

        case NODE_FUNC_DECL: {
            bool success = symtable_insert_func(st, node->val.string_val, node->inferred_type, node->params, node->param_count);
            if (!success) {
                dominik_error(3, "Redefinition of function");
            }

            DataType prev_ret_type = current_func_ret_type;
            current_func_ret_type = node->inferred_type;

            scope_push(st);
            for (int i = 0; i < node->param_count; i++) {
                symtable_insert_var(st, node->params[i].name, node->params[i].type, true);
            }

            check_statement(node->right, st);
            scope_pop(st);

            current_func_ret_type = prev_ret_type;
            break;
        }

        case NODE_BLOCK: {
            scope_push(st);
            check_statement(node->left, st);
            scope_pop(st);
            break;
        }

        default:
            break;
    }

    if (node->next != NULL) {
        check_statement(node->next, st);
    }
}

// evaluates expression nodes bottom-up, validates operand compatibility, checks function calls, and returns the inferred DataType
static DataType check_expression(ASTNode *node, Symtable *st) {
    if (node == NULL) return TYPE_VOID;

    switch (node->type) {
        case NODE_INT_LITERAL:
            node->inferred_type = TYPE_INT;
            return TYPE_INT;

        case NODE_FLOAT_LITERAL:
            node->inferred_type = TYPE_FLOAT;
            return TYPE_FLOAT;

        case NODE_STRING_LITERAL:
            node->inferred_type = TYPE_STRING;
            return TYPE_STRING;

        case NODE_BOOL_LITERAL:
            node->inferred_type = TYPE_BOOL;
            return TYPE_BOOL;

        case NODE_VAR: {
            Symbol *sym = symtable_search(st, node->val.string_val);
            if (sym == NULL) {
                dominik_error(3, "Undefined variable");
            }
            node->inferred_type = sym->data_type;
            return sym->data_type;
        }

        case NODE_ADD:
        case NODE_SUB:
        case NODE_MUL:
        case NODE_DIV: {
            DataType left_type = check_expression(node->left, st);
            DataType right_type = check_expression(node->right, st);

            if (left_type == TYPE_INT && right_type == TYPE_INT) {
                node->inferred_type = TYPE_INT;
                return TYPE_INT;
            } 
            if (left_type == TYPE_FLOAT && right_type == TYPE_FLOAT) {
                node->inferred_type = TYPE_FLOAT;
                return TYPE_FLOAT;
            } 
            if (convert_if_needed(&node->left, left_type, TYPE_FLOAT) && 
                convert_if_needed(&node->right, right_type, TYPE_FLOAT)) {
                node->inferred_type = TYPE_FLOAT;
                return TYPE_FLOAT;
            }
            if (node->type == NODE_ADD && left_type == TYPE_STRING && right_type == TYPE_STRING) {
                node->inferred_type = TYPE_STRING;
                return TYPE_STRING;
            }

            dominik_error(4, "Incompatible types in arithmetic operation");
            break;
        }

        case NODE_EQ:
        case NODE_NEQ:
        case NODE_LT:
        case NODE_GT:
        case NODE_LEQ:
        case NODE_GEQ: {
            DataType left_type = check_expression(node->left, st);
            DataType right_type = check_expression(node->right, st);

            if (left_type == TYPE_INT && right_type == TYPE_FLOAT) {
                convert_if_needed(&node->left, left_type, TYPE_FLOAT);
                left_type = TYPE_FLOAT;
            } else if (left_type == TYPE_FLOAT && right_type == TYPE_INT) {
                convert_if_needed(&node->right, right_type, TYPE_FLOAT);
                right_type = TYPE_FLOAT;
            }

            if (left_type == right_type && left_type != TYPE_VOID && left_type != TYPE_UNKNOWN) {
                node->inferred_type = TYPE_BOOL;
                return TYPE_BOOL;
            }

            dominik_error(4, "Cannot compare incompatible types");
            break;
        }

        case NODE_AND:
        case NODE_OR: {
            DataType left_type = check_expression(node->left, st);
            DataType right_type = check_expression(node->right, st);

            if (left_type == TYPE_BOOL && right_type == TYPE_BOOL) {
                node->inferred_type = TYPE_BOOL;
                return TYPE_BOOL;
            }
            dominik_error(4, "Logical operators require Boolean operands");
            break;
        }

        case NODE_NOT: {
            DataType operand_type = check_expression(node->left, st);
            if (operand_type == TYPE_BOOL) {
                node->inferred_type = TYPE_BOOL;
                return TYPE_BOOL;
            }
            dominik_error(4, "Operator '!' requires Boolean operand");
            break;
        }

        case NODE_CALL: {
            Symbol *func_sym = symtable_search(st, node->val.string_val);
            if (func_sym == NULL || func_sym->sym_type != SYMBOL_FUNC) {
                dominik_error(3, "Undefined function");
            }

            int arg_count = 0;
            ASTNode *arg_node = node->right;

            while (arg_node != NULL) {
                if (arg_count >= func_sym->param_count) {
                    dominik_error(5, "Too many arguments for function");
                }

                DataType arg_type = check_expression(arg_node, st);
                DataType param_type = func_sym->params[arg_count].type;

                if (!convert_if_needed(&arg_node, arg_type, param_type)) {
                    dominik_error(4, "Parameter type mismatch");
                }

                arg_count++;
                arg_node = arg_node->next;
            }

            if (arg_count < func_sym->param_count) {
                dominik_error(5, "Too few arguments for function");
            }

            node->inferred_type = func_sym->data_type;
            return func_sym->data_type;
        }

        default:
            return TYPE_UNKNOWN;
    }
    return TYPE_UNKNOWN;
}