// Compiler of IFJ 2026 imperative language
// Author: Daniel Jurkovič (xjurkod00)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "semantic.h"
// #include "error.h" dominiks error handling

static DataType current_func_ret_type = TYPE_VOID;

// Conversion node allocation (Int -> Float)
static ASTNode* wrap_int2float(ASTNode *expr) {
    if (expr == NULL) return NULL;

    ASTNode *conv = malloc(sizeof(ASTNode));
    if (conv == NULL) {
        dominik_error(99, "Internal Error: Memory allocation failed");
    }

    conv->type = AST_NODE_INT2FLOAT;
    conv->inferred_type = TYPE_FLOAT;
    conv->left = expr;
    conv->right = NULL;
    conv->extra = NULL;
    conv->next = NULL;

    return conv;
}

// Checks if retyping is needed (Int -> Float) for assignments, returns, and arguments
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

// Helper for binary operations: if one operand is Int and the other is Float, promote Int to Float
static void promote_binary_operands_if_needed(ASTNode *node, DataType *left_type, DataType *right_type) {
    if (*left_type == TYPE_INT && *right_type == TYPE_FLOAT) {
        node->left = wrap_int2float(node->left);
        *left_type = TYPE_FLOAT;
    } else if (*left_type == TYPE_FLOAT && *right_type == TYPE_INT) {
        node->right = wrap_int2float(node->right);
        *right_type = TYPE_FLOAT;
    }
}

// Replaces a node's variable identifier with its unique mangled name
static void apply_mangled_name(ASTNode *node, Symbol *sym) {
    if (node != NULL && sym != NULL && sym->mangled_name != NULL) {
        if (node->val.attribute.string_value != NULL) {
            free(node->val.attribute.string_value);
        }
        node->val.attribute.string_value = strdup(sym->mangled_name);
    }
}

// Validates presence and correct signature of the mandatory 'main' function
static void check_main_function(Symtable *st) {
    Symbol *main_sym = symtable_search(st, "main");
    if (main_sym == NULL || main_sym->s_type != SYMBOL_FUNC) {
        dominik_error(3, "Missing mandatory 'main' function");
    }
    if (main_sym->params_amount != 0) {
        dominik_error(4, "'main' function must not take any parameters");
    }
    if (main_sym->d_type != TYPE_VOID) {
        dominik_error(4, "'main' function return type must be void");
    }
}

static void check_statement(ASTNode *node, Symtable *st);
static DataType check_expression(ASTNode *node, Symtable *st);

// Initiates the recursive check on the root of the AST.
bool check_semantics(ASTNode *root, Symtable *st) {
    if (root == NULL) return true;
    check_statement(root, st);
    check_main_function(st);
    return true;
}

// Evaluates arithmetic operations (+, -, *, /)
static DataType check_arithmetic(ASTNode *node, Symtable *st) {
    DataType left_type = check_expression(node->left, st);
    DataType right_type = check_expression(node->right, st);

    if (node->val.type == TOKEN_PLUS && left_type == TYPE_STRING && right_type == TYPE_STRING) {
        node->inferred_type = TYPE_STRING;
        return TYPE_STRING;
    }

    promote_binary_operands_if_needed(node, &left_type, &right_type);

    if (left_type == TYPE_INT && right_type == TYPE_INT) {
        node->inferred_type = TYPE_INT;
        return TYPE_INT;
    }
    if (left_type == TYPE_FLOAT && right_type == TYPE_FLOAT) {
        node->inferred_type = TYPE_FLOAT;
        return TYPE_FLOAT;
    }

    dominik_error(4, "Incompatible types in arithmetic operation");
    return TYPE_UNKNOWN;
}

// Evaluates comparison operations
static DataType check_comparison(ASTNode *node, Symtable *st) {
    DataType left_type = check_expression(node->left, st);
    DataType right_type = check_expression(node->right, st);

    promote_binary_operands_if_needed(node, &left_type, &right_type);

    if (left_type == right_type && left_type != TYPE_VOID && left_type != TYPE_UNKNOWN) {
        node->inferred_type = TYPE_BOOL;
        return TYPE_BOOL;
    }

    dominik_error(4, "Cannot compare incompatible types");
    return TYPE_UNKNOWN;
}

// Evaluates function call expressions
static DataType check_func_call(ASTNode *node, Symtable *st) {
    Symbol *func_sym = symtable_search(st, node->val.attribute.string_value);
    if (func_sym == NULL || func_sym->s_type != SYMBOL_FUNC) {
        dominik_error(3, "Undefined function");
    }

    int arg_count = 0;
    ASTNode **arg_ptr = &node->right;

    while (*arg_ptr != NULL) {
        if (arg_count >= func_sym->params_amount) {
            dominik_error(5, "Too many arguments for function");
        }

        ASTNode *curr_arg = *arg_ptr;
        DataType arg_type = check_expression(curr_arg, st);
        DataType param_type = func_sym->params[arg_count].d_type;

        if (!convert_if_needed(arg_ptr, arg_type, param_type)) {
            dominik_error(4, "Parameter type mismatch");
        }

        if (*arg_ptr != curr_arg) {
            (*arg_ptr)->next = curr_arg->next;
            curr_arg->next = NULL;
        }

        arg_count++;
        arg_ptr = &((*arg_ptr)->next);
    }

    if (arg_count < func_sym->params_amount) {
        dominik_error(5, "Too few arguments for function");
    }

    node->inferred_type = func_sym->d_type;
    return func_sym->d_type;
}

// Recursively checks statement nodes and manages scope stack operations
static void check_statement(ASTNode *node, Symtable *st) {
    if (node == NULL) return;

    switch (node->type) {
        case AST_NODE_PROGRAM: {
            check_statement(node->left, st);
            break;
        }
        case AST_NODE_VAR_DECL: {
            DataType expr_type = check_expression(node->right, st);

            if (node->inferred_type == TYPE_UNKNOWN) {
                node->inferred_type = expr_type;
            }

            if (node->inferred_type == TYPE_VOID || node->inferred_type == TYPE_UNKNOWN) {
                dominik_error(4, "Cannot infer variable type or assign void");
            }

            ASTNode *var_node = node->left;
            bool success = symtable_insert_var(st, var_node->val.attribute.string_value, node->inferred_type, true);
            if (!success) {
                dominik_error(3, "Redefinition of variable");
            }

            Symbol *sym = symtable_search_current_scope(st, var_node->val.attribute.string_value);
            apply_mangled_name(var_node, sym);

            if (!convert_if_needed(&node->right, expr_type, node->inferred_type)) {
                dominik_error(4, "Cannot assign incompatible type to variable");
            }
            break;
        }

        case AST_NODE_ASSIGN: {
            Symbol *sym = symtable_search(st, node->left->val.attribute.string_value);
            if (sym == NULL) {
                dominik_error(3, "Assignment to undefined variable");
            }
            apply_mangled_name(node->left, sym);

            DataType expr_type = check_expression(node->right, st);
            if (!convert_if_needed(&node->right, expr_type, sym->d_type)) {
                dominik_error(4, "Type mismatch in assignment");
            }
            break;
        }

        case AST_NODE_IF: {
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

        case AST_NODE_WHILE: {
            DataType cond_type = check_expression(node->left, st);
            if (cond_type != TYPE_BOOL) {
                dominik_error(4, "Condition in 'while' must be Boolean");
            }

            scope_push(st);
            check_statement(node->right, st);
            scope_pop(st);
            break;
        }

        case AST_NODE_RETURN: {
            DataType return_type = TYPE_VOID;
            if (node->left != NULL) {
                return_type = check_expression(node->left, st);
            }

            if (!convert_if_needed(&node->left, return_type, current_func_ret_type)) {
                dominik_error(4, "Return type mismatch");
            }
            break;
        }

        case AST_NODE_CALL: {
            check_expression(node, st);
            break;
        }

        case AST_NODE_FUNCTION: {
            int params_amount = 0;
            ASTNode *curr = node->left;
            while (curr != NULL) {
                params_amount++;
                curr = curr->next;
            }

            Param *extracted_params = NULL;
            if (params_amount > 0) {
                extracted_params = malloc(params_amount * sizeof(Param));
                curr = node->left;
                for (int i = 0; i < params_amount; i++) {
                    extracted_params[i].name = curr->val.attribute.string_value;
                    extracted_params[i].d_type = curr->inferred_type; 
                    curr = curr->next;
                }
            }

            bool success = symtable_insert_func(st, node->val.attribute.string_value, node->inferred_type, extracted_params, params_amount);
            if (!success) dominik_error(3, "Redefinition of function");

            DataType prev_ret_type = current_func_ret_type;
            current_func_ret_type = node->inferred_type;

            scope_push(st);
            for (int i = 0; i < params_amount; i++) {
                symtable_insert_var(st, extracted_params[i].name, extracted_params[i].d_type, true);
            }

            check_statement(node->right, st);
            scope_pop(st);

            if (extracted_params) free(extracted_params);
            current_func_ret_type = prev_ret_type;
            break;
        }

        default:
            break;
    }

    if (node->next != NULL) {
        check_statement(node->next, st);
    }
}

// Evaluates expression nodes bottom-up, validates operand compatibility, checks function calls, and returns the inferred DataType
static DataType check_expression(ASTNode *node, Symtable *st) {
    if (node == NULL) return TYPE_VOID;

    switch (node->type) {
        case AST_NODE_TERM: {
            if (node->val.type == TOKEN_INTEGER) {
                node->inferred_type = TYPE_INT;
                return TYPE_INT;
            } else if (node->val.type == TOKEN_DOUBLE) {
                node->inferred_type = TYPE_FLOAT;
                return TYPE_FLOAT;
            } else if (node->val.type == TOKEN_STRING) {
                node->inferred_type = TYPE_STRING;
                return TYPE_STRING;
            } else if (node->val.type == TOKEN_IDENTIFIER) {
                Symbol *sym = symtable_search(st, node->val.attribute.string_value);
                if (sym == NULL) dominik_error(3, "Undefined variable");

                apply_mangled_name(node, sym);
                node->inferred_type = sym->d_type;
                return sym->d_type;
            }
            break;
        }

        case AST_NODE_EXPR: {
            if (node->left == NULL && node->right == NULL) {
                if (node->val.type == TOKEN_INTEGER) {
                    node->inferred_type = TYPE_INT;
                    return TYPE_INT;
                } else if (node->val.type == TOKEN_DOUBLE) {
                    node->inferred_type = TYPE_FLOAT;
                    return TYPE_FLOAT;
                } else if (node->val.type == TOKEN_STRING) {
                    node->inferred_type = TYPE_STRING;
                    return TYPE_STRING;
                } else if (node->val.type == TOKEN_IDENTIFIER) {
                    Symbol *sym = symtable_search(st, node->val.attribute.string_value);
                    if (sym == NULL) dominik_error(3, "Undefined variable");

                    apply_mangled_name(node, sym);
                    node->inferred_type = sym->d_type;
                    return sym->d_type;
                }
            }

            if (node->left != NULL && node->right == NULL) {
                if (node->val.type == TOKEN_PLUS || node->val.type == TOKEN_MINUS) {
                    DataType operand_type = check_expression(node->left, st);
                    if (operand_type == TYPE_INT || operand_type == TYPE_FLOAT) {
                        node->inferred_type = operand_type;
                        return operand_type;
                    }
                    dominik_error(4, "Unary +/- requires numeric operand");
                }
            }

            switch (node->val.type) {
                case TOKEN_PLUS:
                case TOKEN_MINUS:
                case TOKEN_MUL:
                case TOKEN_DIV:
                    return check_arithmetic(node, st);

                case TOKEN_LESS:
                case TOKEN_LESS_EQ:
                case TOKEN_GREATER:
                case TOKEN_GREATER_EQ:
                case TOKEN_EQ:
                case TOKEN_NOT_EQ:
                    return check_comparison(node, st);

                default:
                    dominik_error(99, "Unknown operator in expression");
            }
            break;
        }

        case AST_NODE_CALL:
            return check_func_call(node, st);

        default:
            return TYPE_UNKNOWN;
    }
    return TYPE_UNKNOWN;
}