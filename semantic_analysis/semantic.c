// Compiler of IFJ 2026 imperative language
// Author: Daniel Jurkovič (xjurkod00)

#include <stdio.h>
#include "semantic.h"
void dominik_error(int code, const char *msg); //dočásné riešenie
static DataType current_func_ret_type = TYPE_VOID;

static void check_statement(ASTNode *node, Symtable *st);
/**
 * @brief Allocates and initializes an AST conversion node (Int -> Float).
 * @param expr Pointer to the integer expression AST node to be wrapped.
 * @return Pointer to the newly created AST_NODE_INT2FLOAT node, or NULL if expr is NULL.
 */
static ASTNode* wrap_int2float(ASTNode *expr) {
    if (expr == NULL) return NULL;

    Token t;
    memset(&t, 0, sizeof(Token));
    ASTNode *conv = create_ast_node(AST_NODE_INT2FLOAT, expr, NULL, NULL, t);
    conv->inferred_type = TYPE_FLOAT;
    conv->prev = expr->prev;
    return conv;
}
/**
 * @brief Validates type compatibility and performs implicit Int-to-Float conversion if needed.
 * @param expr_ptr Address of the pointer to the expression AST node (modified in-place if wrapped).
 * @param actual_type Evaluated data type of the expression.
 * @param target_type Expected target data type.
 * @return true if types match or conversion succeeds; false on incompatible types.
 */
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

/**
 * @brief Promotes integer operand to float in mixed Int/Float binary expressions.
 * @param node Parent binary expression AST node.
 * @param left_type Pointer to the left operand's data type (updated if promoted).
 * @param right_type Pointer to the right operand's data type (updated if promoted).
 */
static void promote_binary_operands_if_needed(ASTNode *node, DataType *left_type, DataType *right_type) {
    if (*left_type == TYPE_INT && *right_type == TYPE_FLOAT) {
        node->left = wrap_int2float(node->left);
        *left_type = TYPE_FLOAT;
    } else if (*left_type == TYPE_FLOAT && *right_type == TYPE_INT) {
        node->right = wrap_int2float(node->right);
        *right_type = TYPE_FLOAT;
    }
}

/**
 * @brief Replaces an AST node's identifier string with its unique scope-mangled name.
 * @param node AST node holding the identifier token.
 * @param sym Symbol table entry containing the generated mangled name.
 */
static void apply_mangled_name(ASTNode *node, Symbol *sym) {
    if (node != NULL && sym != NULL && sym->mangled_name != NULL) {
        if (node->val.attribute.string_value != NULL) {
            free(node->val.attribute.string_value);
        }
        node->val.attribute.string_value = str_duplicate(sym->mangled_name);
    }
}

/**
 * @brief Pre-registers all 10 built-in IFJ26 functions into the global symbol table.
 * @param st Pointer to the symbol table.
 */
static void register_builtin_functions(Symtable *st) {
    symtable_insert_func(st, "readLine", TYPE_STRING, NULL, 0);
    symtable_insert_func(st, "readInt", TYPE_INT, NULL, 0);
    symtable_insert_func(st, "readDouble", TYPE_FLOAT, NULL, 0);
    
    // Variadic/polymorphic 'Any' parameter types are handled separately in check_func_call
    symtable_insert_func(st, "print", TYPE_VOID, NULL, 1);
    symtable_insert_func(st, "toStr", TYPE_STRING, NULL, 1);

    Param p_double[1] = { { .name = "value", .d_type = TYPE_FLOAT } };
    symtable_insert_func(st, "toInt", TYPE_INT, p_double, 1);

    Param p_str[1] = { { .name = "value", .d_type = TYPE_STRING } };
    symtable_insert_func(st, "strLength", TYPE_INT, p_str, 1);

    Param p_substr[3] = {
        { .name = "value", .d_type = TYPE_STRING },
        { .name = "start", .d_type = TYPE_INT },
        { .name = "end",   .d_type = TYPE_INT }
    };
    symtable_insert_func(st, "subStr", TYPE_STRING, p_substr, 3);

    Param p_ord[2] = {
        { .name = "value", .d_type = TYPE_STRING },
        { .name = "index", .d_type = TYPE_INT }
    };
    symtable_insert_func(st, "ord", TYPE_INT, p_ord, 2);

    Param p_int[1] = { { .name = "code", .d_type = TYPE_INT } };
    symtable_insert_func(st, "chr", TYPE_STRING, p_int, 1);
}

/**
 * @brief Recursively verifies that the final statement or all branches of an ending 'if-else'
 *        produce a valid implicit return value matching the function's expected return type.
 * @param stmt_list_ptr Address of the pointer to the statement list head.
 * @param expected_ret_type Expected return DataType of the enclosing function.
 * @return true if a valid explicit or implicit return is guaranteed; false otherwise.
 */
static bool check_implicit_return(ASTNode **stmt_list_ptr, DataType expected_ret_type) {
    ASTNode **curr_ptr = stmt_list_ptr;
    ASTNode **last_ptr = NULL;

    while (*curr_ptr != NULL) {
        last_ptr = curr_ptr;
        curr_ptr = &((*curr_ptr)->next);
    }

    if (last_ptr == NULL) {
        return false;
    }

    ASTNode *last = *last_ptr;
    if (last->type == AST_NODE_RETURN) {
        return true;
    }

    if (last->type == AST_NODE_EXPR || last->type == AST_NODE_TERM ||
        last->type == AST_NODE_CALL || last->type == AST_NODE_UNIT) {
        return convert_if_needed(last_ptr, last->inferred_type, expected_ret_type);
    }

    if (last->type == AST_NODE_IF && last->extra != NULL) {
        return check_implicit_return(&last->right, expected_ret_type) &&
               check_implicit_return(&last->extra, expected_ret_type);
    }

    return false;
}

/**
 * @brief Validates the presence, uniqueness, and signature of the mandatory '@main' entry function.
 * @param root Root AST node of the program.
 * @param st Pointer to the symbol table.
 */
static void check_main_function(ASTNode *root) {
    if (root == NULL || root->type != AST_NODE_PROGRAM) return;

    int main_count = 0;
    ASTNode *func = root->left;
    while (func != NULL) {
        if (func->type == AST_NODE_FUNCTION && func->is_main) {
            main_count++;
            
            // Count parameters linked in func->left; '@main' must take 0 parameters
            int params_amount = 0;
            ASTNode *curr = func->left;
            while (curr != NULL) { params_amount++; curr = curr->next; }
            
            if (params_amount != 0) {
                dominik_error(5, "'@main' function must not take any parameters");
            }
            if (func->inferred_type != TYPE_VOID) {
                dominik_error(6, "'@main' function return type must be void (Unit)");
            }
        }
        func = func->next;
    }

    if (main_count == 0) {
        dominik_error(3, "Missing mandatory '@main' function");
    }
    if (main_count > 1) {
        dominik_error(4, "Multiple '@main' functions detected");
    }
}

/**
 * @brief Deduces the return DataType from the final statement or expression of a function body
 *        when the explicit return type annotation was omitted in the function header.
 * @param stmt_list Head of the function body's statement list.
 * @return Deduced DataType (TYPE_VOID for block bodies without a trailing expression).
 */
static DataType deduce_return_type(ASTNode *stmt_list) {
    ASTNode *last = stmt_list;
    while (last != NULL && last->next != NULL) {
        last = last->next;
    }

    if (last == NULL) {
        return TYPE_VOID;
    }

    if (last->type == AST_NODE_RETURN) {
        return (last->left != NULL) ? last->left->inferred_type : TYPE_VOID;
    }

    if (last->type == AST_NODE_EXPR || last->type == AST_NODE_TERM ||
        last->type == AST_NODE_CALL || last->type == AST_NODE_UNIT) {
        return last->inferred_type;
    }

    if (last->type == AST_NODE_IF && last->extra != NULL) {
        DataType then_type = deduce_return_type(last->right);
        DataType else_type = deduce_return_type(last->extra);

        if (then_type == else_type) {
            return then_type;
        }
        if ((then_type == TYPE_INT && else_type == TYPE_FLOAT) ||
            (then_type == TYPE_FLOAT && else_type == TYPE_INT)) {
            return TYPE_FLOAT;
        }
        return TYPE_UNKNOWN;
    }

    return TYPE_VOID;
}

#define MAX_TRACKED_FUNCS 1024
static ASTNode *program_root = NULL;
static ASTNode *checked_funcs[MAX_TRACKED_FUNCS];
static int checked_funcs_count = 0;
static ASTNode *visiting_funcs[MAX_TRACKED_FUNCS];
static int visiting_funcs_count = 0;

static bool is_in_func_list(ASTNode **list, int count, ASTNode *target) {
    for (int i = 0; i < count; i++) {
        if (list[i] == target) return true;
    }
    return false;
}

static ASTNode* find_func_node(const char *name) {
    if (program_root == NULL || program_root->type != AST_NODE_PROGRAM) return NULL;
    ASTNode *curr = program_root->left;
    while (curr != NULL) {
        if (curr->type == AST_NODE_FUNCTION &&
            strcmp(curr->val.attribute.string_value, name) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

/**
 * @brief Validates a function's parameters, body, and explicit or inferred return type,
 *        supporting on-demand evaluation for forward function calls.
 * @param node Function definition AST node (AST_NODE_FUNCTION).
 * @param st Pointer to the symbol table.
 */
static void check_function(ASTNode *node, Symtable *st) {
    if (node == NULL || is_in_func_list(checked_funcs, checked_funcs_count, node)) {
        return;
    }

    if (visiting_funcs_count < MAX_TRACKED_FUNCS) {
        visiting_funcs[visiting_funcs_count++] = node;
    }

    Symbol *func_sym = symtable_search(st, node->val.attribute.string_value);
    bool infer_return = (node->inferred_type == TYPE_UNKNOWN);

    DataType prev_ret_type = current_func_ret_type;
    current_func_ret_type = node->inferred_type;

    bool prev_in_main = st->in_main;
    st->in_main = node->is_main;

    scope_push(st);

    ASTNode *param_node = node->left;
    if (func_sym != NULL && func_sym->params != NULL) {
        for (int i = 0; i < func_sym->params_amount; i++) {
            if (!symtable_insert_var(st, func_sym->params[i].name, func_sym->params[i].d_type, true, true)) {
                dominik_error(4, "Redefinition of function parameter");
            }
            Symbol *p_sym = symtable_search_current_scope(st, func_sym->params[i].name);
            apply_mangled_name(param_node, p_sym);
            if (param_node != NULL) {
                param_node = param_node->next;
            }
        }
    }

    check_statement(node->right, st);

    if (infer_return) {
        DataType deduced = deduce_return_type(node->right);
        if (deduced == TYPE_UNKNOWN || deduced == TYPE_BOOL) {
            dominik_error(6, "Cannot infer valid return type for function");
        }
        if (current_func_ret_type != TYPE_UNKNOWN && current_func_ret_type != deduced) {
            dominik_error(6, "Conflicting return types in function");
        }
        node->inferred_type = deduced;
        if (func_sym != NULL) {
            func_sym->d_type = deduced;
        }
        current_func_ret_type = deduced;
    }

    if (current_func_ret_type != TYPE_VOID) {
        if (!check_implicit_return(&node->right, current_func_ret_type)) {
            dominik_error(6, "Missing return statement or implicit return type mismatch");
        }
    }

    scope_pop(st);

    st->in_main = prev_in_main;
    current_func_ret_type = prev_ret_type;

    visiting_funcs_count--;
    if (checked_funcs_count < MAX_TRACKED_FUNCS) {
        checked_funcs[checked_funcs_count++] = node;
    }
}

static DataType check_expression(ASTNode *node, Symtable *st);

/**
 * @brief Entry point for semantic analysis; registers functions and validates the entire AST.
 * @param root Root node of the Abstract Syntax Tree.
 * @param st Pointer to the initialized symbol table.
 * @return true if semantic analysis succeeds without errors.
 */
bool check_semantics(ASTNode *root, Symtable *st) {
    if (root == NULL) return true;

    program_root = root;
    checked_funcs_count = 0;
    visiting_funcs_count = 0;

    register_builtin_functions(st);

    // First pass: register all user-defined function signatures to allow forward calls and recursion
    if (root->type == AST_NODE_PROGRAM) {
        ASTNode *func = root->left;
        while (func != NULL) {
            if (func->type == AST_NODE_FUNCTION) {
                int params_amount = 0;
                ASTNode *curr = func->left;
                while (curr != NULL) {
                    params_amount++;
                    curr = curr->next;
                }

                // Extract linked parameter AST nodes into a contiguous Param array for symbol table insertion
                Param *extracted_params = NULL;
                if (params_amount > 0) {
                    extracted_params = malloc(params_amount * sizeof(Param));
                    curr = func->left;
                    for (int i = 0; i < params_amount; i++) {
                        extracted_params[i].name = curr->val.attribute.string_value;
                        extracted_params[i].d_type = curr->inferred_type; 
                        curr = curr->next;
                    }
                }

                bool success = symtable_insert_func(st, func->val.attribute.string_value, func->inferred_type, extracted_params, params_amount);
                if (extracted_params) free(extracted_params);
                if (!success) dominik_error(3, "Redefinition of function");
            }
            func = func->next;
        }
    }

    // Second pass: recursively validate all function bodies and statements, then verify '@main'
    check_statement(root, st);
    check_main_function(root);
    return true;
}

/**
 * @brief Evaluates binary arithmetic operations (+, -, *, /) and string concatenation (+).
 * @param node Binary expression AST node.
 * @param st Pointer to the symbol table.
 * @return Resulting DataType of the arithmetic expression.
 */
static DataType check_arithmetic(ASTNode *node, Symtable *st) {
    DataType left_type = check_expression(node->left, st);
    DataType right_type = check_expression(node->right, st);

    // String concatenation is valid exclusively for '+' with two String operands
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

    dominik_error(6, "Incompatible types in arithmetic operation");
    return TYPE_UNKNOWN;
}

/**
 * @brief Evaluates relational and equality comparison operations (<, <=, >, >=, ==, !=).
 * @param node Comparison expression AST node.
 * @param st Pointer to the symbol table.
 * @return TYPE_BOOL if operands are compatible; triggers semantic error 6 otherwise.
 */
static DataType check_comparison(ASTNode *node, Symtable *st) {
    DataType left_type = check_expression(node->left, st);
    DataType right_type = check_expression(node->right, st);

    promote_binary_operands_if_needed(node, &left_type, &right_type);

    bool is_equality = (node->val.type == TOKEN_EQ || node->val.type == TOKEN_NOT_EQ);

    if (left_type == right_type) {
        if (left_type == TYPE_INT || left_type == TYPE_FLOAT || left_type == TYPE_STRING) {
            node->inferred_type = TYPE_BOOL;
            return TYPE_BOOL;
        }
        if (is_equality && left_type == TYPE_VOID) {
            node->inferred_type = TYPE_BOOL;
            return TYPE_BOOL;
        }
    }

    dominik_error(6, "Cannot compare incompatible types");
    return TYPE_UNKNOWN;
}

/**
 * @brief Validates function calls, argument counts, and argument types (including built-ins).
 * @param node Function call AST node (AST_NODE_CALL).
 * @param st Pointer to the symbol table.
 * @return Return DataType of the called function.
 */
static DataType check_func_call(ASTNode *node, Symtable *st) {
    char *func_name = node->val.attribute.string_value;

    // Special handling for built-in 'print' and 'toStr' accepting a single argument of type Any
    if (strcmp(func_name, "print") == 0 || strcmp(func_name, "toStr") == 0) {
        ASTNode *arg_node = node->right;
        if (arg_node == NULL || arg_node->next != NULL) {
            dominik_error(5, "Built-in function requires exactly one argument");
        }
        DataType arg_type = check_expression(arg_node, st);
        if (arg_type == TYPE_UNKNOWN || arg_type == TYPE_BOOL) {
            dominik_error(5, "Invalid argument type for built-in function");
        }
        DataType ret = (strcmp(func_name, "print") == 0) ? TYPE_VOID : TYPE_STRING;
        node->inferred_type = ret;
        return ret;
    }

    Symbol *func_sym = symtable_search(st, func_name);
    if (func_sym == NULL || func_sym->s_type != SYMBOL_FUNC) {
        dominik_error(3, "Undefined function");
    }

    //If the function does not have a specific return type defined, evaluate
    if (func_sym->d_type == TYPE_UNKNOWN) {
        ASTNode *target_func = find_func_node(func_name);
        if (target_func == NULL || is_in_func_list(visiting_funcs, visiting_funcs_count, target_func)) {
            dominik_error(6, "Cannot infer return type of recursive function");
        }
        check_function(target_func, st);
    }

    // Iterate through linked argument list via pointer-to-pointer to allow in-place conversion wrapping
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
            dominik_error(5, "Parameter type mismatch");
        }

        //preserve the sibling chain if the argument node was wrapped in an AST_NODE_INT2FLOAT node
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

/**
 * @brief Recursively traverses and validates statement AST nodes and manages lexical scopes.
 * @param node Current statement AST node.
 * @param st Pointer to the symbol table.
 */
static void check_statement(ASTNode *node, Symtable *st) {
    if (node == NULL) return;

    switch (node->type) {
        case AST_NODE_PROGRAM: {
            check_statement(node->left, st);
            break;
        }
        case AST_NODE_VAR_DECL: {
            DataType expr_type = check_expression(node->right, st);

            // Infer variable type from initializer expression if no explicit type was given
            if (node->inferred_type == TYPE_UNKNOWN) {
                node->inferred_type = expr_type;
            }

                //node->inferred_type == TYPE_VOID || this was deleted
            if (node->inferred_type == TYPE_UNKNOWN || node->inferred_type == TYPE_BOOL) {
                dominik_error(6, "Cannot infer variable type or assign void/boolean");
            }

            ASTNode *var_node = node->left;
            bool success = symtable_insert_var(st, var_node->val.attribute.string_value, node->inferred_type, true, node->is_const);
            if (!success) {
                dominik_error(3, "Redefinition of variable");
            }

            Symbol *sym = symtable_search_current_scope(st, var_node->val.attribute.string_value);
            apply_mangled_name(var_node, sym);

            if (!convert_if_needed(&node->right, expr_type, node->inferred_type)) {
                dominik_error(6, "Cannot assign incompatible type to variable");
            }
            break;
        }

        case AST_NODE_ASSIGN: {
            Symbol *sym = symtable_search(st, node->left->val.attribute.string_value);
            if (sym == NULL|| sym->s_type != SYMBOL_VAR) {
                dominik_error(3, "Assignment to undefined variable");
            }
            // Prevent reassignment to immutable 'val' variables or formal function parameters
            if (sym->is_const) {
                dominik_error(7, "Assignment to constant (val) variable or parameter");
            }

            apply_mangled_name(node->left, sym);

            DataType expr_type = check_expression(node->right, st);
            if (!convert_if_needed(&node->right, expr_type, sym->d_type)) {
                dominik_error(6, "Type mismatch in assignment");
            }
            break;
        }

        case AST_NODE_IF: {
            DataType cond_type = check_expression(node->left, st);
            if (cond_type != TYPE_BOOL) {
                dominik_error(6, "Condition in 'if' must be Boolean");
            }

            // Evaluate 'then' and optional 'else' branches in isolated lexical scopes
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
                dominik_error(6, "Condition in 'while' must be Boolean");
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

            if (current_func_ret_type == TYPE_UNKNOWN) {
                dominik_error(6, "Function containing 'return' must have explicit return type");
                current_func_ret_type = return_type;
            }
            else if (current_func_ret_type == TYPE_VOID) {
                if (return_type == TYPE_UNKNOWN || return_type == TYPE_BOOL) {
                    dominik_error(6, "Invalid return value");
                }
            }
            else if (!convert_if_needed(&node->left, return_type, current_func_ret_type)) {
                dominik_error(6, "Return type mismatch");
            }
            break;
        }
        case AST_NODE_CALL: {
            check_expression(node, st);
            break;
        }

        case AST_NODE_EXPR:
        case AST_NODE_TERM:
        case AST_NODE_UNIT: {
            check_expression(node, st);
            break;
        }

        case AST_NODE_FUNCTION: {
            check_function(node, st);
            break;
        }

        default:
            break;
    }

    if (node->next != NULL) {
        check_statement(node->next, st);
    }
}

/**
 * @brief Recursively evaluates expression AST nodes bottom-up and annotates inferred types.
 * @param node Current expression AST node.
 * @param st Pointer to the symbol table.
 * @return Inferred DataType of the evaluated expression.
 */
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
                if (sym == NULL|| sym->s_type != SYMBOL_VAR) dominik_error(3, "Undefined variable");

                apply_mangled_name(node, sym);
                node->inferred_type = sym->d_type;
                return sym->d_type;
            }
            break;
        }

        case AST_NODE_EXPR: {
            // Handle leaf expression nodes wrapping a single literal or variable identifier
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
                    if (sym == NULL || sym->s_type != SYMBOL_VAR) dominik_error(3, "Undefined variable");

                    apply_mangled_name(node, sym);
                    node->inferred_type = sym->d_type;
                    return sym->d_type;
                }
            }

            // Handle unary '+' and '-' operators
            if (node->left != NULL && node->right == NULL) {
                if (node->val.type == TOKEN_PLUS || node->val.type == TOKEN_MINUS) {
                    DataType operand_type = check_expression(node->left, st);
                    if (operand_type == TYPE_INT || operand_type == TYPE_FLOAT) {
                        node->inferred_type = operand_type;
                        return operand_type;
                    }
                    dominik_error(6, "Unary +/- requires numeric operand");
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

        case AST_NODE_UNIT:
            node->inferred_type = TYPE_VOID;
            return TYPE_VOID;

        default:
            return TYPE_UNKNOWN;
    }
    return TYPE_UNKNOWN;
}