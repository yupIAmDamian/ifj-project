#include <stdio.h>
#include <stdlib.h>
#include "semantic.h"

static void check_statement(ASTNode *node, Symtable *st);
static DataType check_expression(ASTNode *node, Symtable *st)TODO

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

        case NODE_ASSIGN: {
            Symbol *sym = symtable_search(st, node->left->val.string_val);
            if (sym == NULL) {
                fprintf(stderr, "Semantic Error: Assignment to undefined variable '%s'\n", node->left->val.string_val);
                exit(3);
            }

            DataType expr_type = check_expression(node->right, st);
            if (sym->data_type != expr_type) {
                if (sym->data_type == TYPE_FLOAT && expr_type == TYPE_INT) {
                    ASTNode *conv = malloc(sizeof(ASTNode));
                    conv->type = NODE_INT2FLOAT;
                    conv->left = node->right;
                    conv->inferred_type = TYPE_FLOAT;
                    node->right = conv;
                } else {
                    fprintf(stderr, "Semantic Error: Type mismatch in assignment to '%s'\n", sym->name);
                    exit(4);
                }
            }
            break;
        }

        case NODE_WHILE: {
            DataType cond_type = check_expression(node->left, st);
            if (cond_type != TYPE_BOOL) {
                fprintf(stderr, "Semantic Error: Condition in 'while' must be Boolean\n");
                exit(4); //type fault
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

            if (current_func_ret_type != return_type) {
                if (current_func_ret_type == TYPE_FLOAT && return_type == TYPE_INT) {
                    ASTNode *conv = malloc(sizeof(ASTNode));
                    conv->type = NODE_INT2FLOAT;
                    conv->left = node->left;
                    conv->inferred_type = TYPE_FLOAT;
                    node->left = conv;
                } else {
                    fprintf(stderr, "Semantic Error: Return type mismatch\n");
                    exit(4); // type incompatibility
                }
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
                fprintf(stderr, "Semantic Error: Redefinition of function '%s'\n", node->val.string_val);
                exit(3); // fault 3 redefine
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
                fprintf(stderr, "Semantic Error: Undefined variable '%s'\n", node->val.string_val);
                exit(3);
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
            else if (left_type == TYPE_FLOAT && right_type == TYPE_FLOAT) {
                node->inferred_type = TYPE_FLOAT;
                return TYPE_FLOAT;
            } 
            else if (left_type == TYPE_INT && right_type == TYPE_FLOAT) {
                ASTNode *conv = malloc(sizeof(ASTNode));
                conv->type = NODE_INT2FLOAT;
                conv->left = node->left;
                conv->inferred_type = TYPE_FLOAT;
                
                node->left = conv;
                node->inferred_type = TYPE_FLOAT;
                return TYPE_FLOAT;
            }
            else if (left_type == TYPE_FLOAT && right_type == TYPE_INT) {
                ASTNode *conv = malloc(sizeof(ASTNode));
                conv->type = NODE_INT2FLOAT;
                conv->left = node->right;
                conv->inferred_type = TYPE_FLOAT;
                
                node->right = conv;
                node->inferred_type = TYPE_FLOAT;
                return TYPE_FLOAT;
            }
            else if (node->type == NODE_ADD && left_type == TYPE_STRING && right_type == TYPE_STRING) {
                node->inferred_type = TYPE_STRING;
                return TYPE_STRING;
            } 
            else {
                fprintf(stderr, "Semantic Error: Incompatible types in arithmetic operation\n");
                exit(4);
            }
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
                ASTNode *conv = malloc(sizeof(ASTNode));
                conv->type = NODE_INT2FLOAT;
                conv->left = node->left;
                conv->inferred_type = TYPE_FLOAT;
                node->left = conv;
                left_type = TYPE_FLOAT;
            } else if (left_type == TYPE_FLOAT && right_type == TYPE_INT) {
                ASTNode *conv = malloc(sizeof(ASTNode));
                conv->type = NODE_INT2FLOAT;
                conv->left = node->right;
                conv->inferred_type = TYPE_FLOAT;
                node->right = conv;
                right_type = TYPE_FLOAT;
            }

            if (left_type == right_type && left_type != TYPE_VOID && left_type != TYPE_UNKNOWN) {
                node->inferred_type = TYPE_BOOL;
                return TYPE_BOOL;
            } else {
                fprintf(stderr, "Semantic Error: Cannot compare incompatible types\n");
                exit(4);
            }
        }

        case NODE_AND:
        case NODE_OR: {
            DataType left_type = check_expression(node->left, st);
            DataType right_type = check_expression(node->right, st);

            if (left_type == TYPE_BOOL && right_type == TYPE_BOOL) {
                node->inferred_type = TYPE_BOOL;
                return TYPE_BOOL;
            } else {
                fprintf(stderr, "Semantic Error: Logical operators require Boolean operands\n");
                exit(4);
            }
        }

        case NODE_NOT: {
            DataType operand_type = check_expression(node->left, st);
            if (operand_type == TYPE_BOOL) {
                node->inferred_type = TYPE_BOOL;
                return TYPE_BOOL;
            } else {
                fprintf(stderr, "Semantic Error: Operator '!' requires Boolean operand\n");
                exit(4);
            }
        }

        case NODE_CALL: {
            Symbol *func_sym = symtable_search(st, node->val.string_val);
            if (func_sym == NULL || func_sym->sym_type != SYMBOL_FUNC) {
                fprintf(stderr, "Semantic Error: Undefined function '%s'\n", node->val.string_val);
                exit(3); // Undeclared function
            }

            int arg_count = 0;
            ASTNode *arg_node = node->right; // !i assume that, this will be saved in right subtree!, bud this depends on miškos implementation

            while (arg_node != NULL) {
                if (arg_count >= func_sym->param_count) {
                    fprintf(stderr, "Semantic Error: Too many arguments for function '%s'\n", func_sym->name);
                    exit(5); // not the right amounts of arguments
                }

                DataType arg_type = check_expression(arg_node, st);
                DataType param_type = func_sym->params[arg_count].type;

                if (arg_type != param_type) {
                    // Implicit conversion
                    if (param_type == TYPE_FLOAT && arg_type == TYPE_INT) {
                        // !depends on the exact structure of your argument concatenation!
                    } else {
                        fprintf(stderr, "Semantic Error: Parameter %d type mismatch in '%s'\n", arg_count + 1, func_sym->name);
                        exit(4); // type incompatibility
                    }
                }

                arg_count++;
                arg_node = arg_node->next;
            }

            if (arg_count < func_sym->param_count) {
                fprintf(stderr, "Semantic Error: Too few arguments for function '%s'\n", func_sym->name);
                exit(5); // not the right amounts of arguments
            }

            node->inferred_type = func_sym->data_type;
            return func_sym->data_type; // expression takes on the return type of the function
        }

        default:
            return TYPE_UNKNOWN;
    }
}