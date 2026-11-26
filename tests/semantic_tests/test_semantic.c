#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../../semantic_analysis/semantic.h"
#include "../../syntactic_analysis/ast.h"

// Global pointers within the test so the error handler can clean them up on exit
static Symtable *g_st = NULL;
static ASTNode *g_root = NULL;

// Helper function for creating a Token with the specified values.
static Token make_tok(TokenType type, char *str_val, int int_val, double dbl_val) {
    Token t;
    memset(&t, 0, sizeof(Token));
    t.type = type;
    if (str_val) {
        t.attribute.string_value = str_duplicate(str_val);
    } else if (type == TOKEN_INTEGER) {
        t.attribute.integer_value = int_val;
    } else if (type == TOKEN_DOUBLE) {
        t.attribute.double_value = dbl_val;
    }
    return t;
}

// Helper function for creating an AST node and initializing its semantic attributes.
static ASTNode* make_node(ASTNodeType type, ASTNode *l, ASTNode *r, ASTNode *ex, Token val, DataType dtype, bool is_const) {
    ASTNode *n = create_ast_node(type, l, r, ex, val);
    n->inferred_type = dtype;
    n->is_const = is_const;
    return n;
}

ASTNode* build_test_ast(void) {

    // FUNCTION 2: def vypocitaj(a: Int, b: Double): Double
    // Create function parameters: a (Int) and b (Double).
    ASTNode *p_a = make_node(AST_NODE_PARAM, NULL, NULL, NULL, make_tok(TOKEN_IDENTIFIER, "a", 0, 0), TYPE_INT, true);
    ASTNode *p_b = make_node(AST_NODE_PARAM, NULL, NULL, NULL, make_tok(TOKEN_IDENTIFIER, "b", 0, 0), TYPE_FLOAT, true);
    p_a->next = p_b;

    // Create the expression a + b.
    // Semantic analysis is expected to insert an INT-to-FLOAT conversion for 'a'.
    ASTNode *var_a1 = make_node(AST_NODE_TERM, NULL, NULL, NULL, make_tok(TOKEN_IDENTIFIER, "a", 0, 0), TYPE_UNKNOWN, false);
    ASTNode *var_b1 = make_node(AST_NODE_TERM, NULL, NULL, NULL, make_tok(TOKEN_IDENTIFIER, "b", 0, 0), TYPE_UNKNOWN, false);
    ASTNode *add_ab = make_node(AST_NODE_EXPR, var_a1, var_b1, NULL, make_tok(TOKEN_PLUS, NULL, 0, 0), TYPE_UNKNOWN, false);
    ASTNode *ret_then = make_node(AST_NODE_RETURN, add_ab, NULL, NULL, make_tok(TOKEN_RETURN, NULL, 0, 0), TYPE_UNKNOWN, false);

    // Create the alternative return statement: return 0.0.
    ASTNode *lit_0 = make_node(AST_NODE_TERM, NULL, NULL, NULL, make_tok(TOKEN_DOUBLE, NULL, 0, 0.0), TYPE_FLOAT, false);
    ASTNode *ret_else = make_node(AST_NODE_RETURN, lit_0, NULL, NULL, make_tok(TOKEN_RETURN, NULL, 0, 0), TYPE_UNKNOWN, false);

    // Create the condition: if (a > 0).
    ASTNode *var_a2 = make_node(AST_NODE_TERM, NULL, NULL, NULL, make_tok(TOKEN_IDENTIFIER, "a", 0, 0), TYPE_UNKNOWN, false);
    ASTNode *lit_int0 = make_node(AST_NODE_TERM, NULL, NULL, NULL, make_tok(TOKEN_INTEGER, NULL, 0, 0), TYPE_INT, false);
    ASTNode *cond_vyp = make_node(AST_NODE_EXPR, var_a2, lit_int0, NULL, make_tok(TOKEN_GREATER, NULL, 0, 0), TYPE_UNKNOWN, false);
    ASTNode *if_vyp = make_node(AST_NODE_IF, cond_vyp, ret_then, ret_else, make_tok(TOKEN_IF, NULL, 0, 0), TYPE_UNKNOWN, false);

    // Create the function node and mark it as a regular function (not main).
    ASTNode *func_vypocitaj = make_node(AST_NODE_FUNCTION, p_a, if_vyp, NULL, make_tok(TOKEN_IDENTIFIER, "vypocitaj", 0, 0), TYPE_FLOAT, false);
    func_vypocitaj->is_main = false;


    // FUNCTION 1: @main def main(): Unit

    // 1. Declare an immutable variable 'zaklad' of type Int and initialize it using readInt().
    ASTNode *id_zaklad = make_node(AST_NODE_TERM, NULL, NULL, NULL, make_tok(TOKEN_IDENTIFIER, "zaklad", 0, 0), TYPE_INT, true);
    ASTNode *call_read = make_node(AST_NODE_CALL, NULL, NULL, NULL, make_tok(TOKEN_IDENTIFIER, "readInt", 0, 0), TYPE_UNKNOWN, false);
    ASTNode *decl_zaklad = make_node(AST_NODE_VAR_DECL, id_zaklad, call_read, NULL, make_tok(TOKEN_IDENTIFIER, "zaklad", 0, 0), TYPE_INT, true);

    // 2. Declare a mutable Double variable 'vysledok' and initialize it
    // with the result of vypocitaj(zaklad, 2.5).
    ASTNode *id_vysledok = make_node(AST_NODE_TERM, NULL, NULL, NULL, make_tok(TOKEN_IDENTIFIER, "vysledok", 0, 0), TYPE_FLOAT, false);
    ASTNode *arg1 = make_node(AST_NODE_TERM, NULL, NULL, NULL, make_tok(TOKEN_IDENTIFIER, "zaklad", 0, 0), TYPE_UNKNOWN, false);
    ASTNode *arg2 = make_node(AST_NODE_TERM, NULL, NULL, NULL, make_tok(TOKEN_DOUBLE, NULL, 0, 2.5), TYPE_FLOAT, false);
    arg1->next = arg2;
    ASTNode *call_vyp = make_node(AST_NODE_CALL, NULL, arg1, NULL, make_tok(TOKEN_IDENTIFIER, "vypocitaj", 0, 0), TYPE_UNKNOWN, false);
    ASTNode *decl_vysledok = make_node(AST_NODE_VAR_DECL, id_vysledok, call_vyp, NULL, make_tok(TOKEN_IDENTIFIER, "vysledok", 0, 0), TYPE_FLOAT, false);

    // 3. THEN branch: declare a local variable with the same name as
    // the outer variable 'zaklad'. This tests variable shadowing.
    ASTNode *id_zaklad_in = make_node(AST_NODE_TERM, NULL, NULL, NULL, make_tok(TOKEN_IDENTIFIER, "zaklad", 0, 0), TYPE_FLOAT, false);
    ASTNode *lit_99 = make_node(AST_NODE_TERM, NULL, NULL, NULL, make_tok(TOKEN_DOUBLE, NULL, 0, 99.9), TYPE_FLOAT, false);
    ASTNode *decl_zaklad_inner = make_node(AST_NODE_VAR_DECL, id_zaklad_in, lit_99, NULL, make_tok(TOKEN_IDENTIFIER, "zaklad", 0, 0), TYPE_FLOAT, false);

    // Create an assignment: vysledok = zaklad + 1.
    ASTNode *lhs_vysl = make_node(AST_NODE_TERM, NULL, NULL, NULL, make_tok(TOKEN_IDENTIFIER, "vysledok", 0, 0), TYPE_UNKNOWN, false);
    ASTNode *rhs_zak = make_node(AST_NODE_TERM, NULL, NULL, NULL, make_tok(TOKEN_IDENTIFIER, "zaklad", 0, 0), TYPE_UNKNOWN, false);
    ASTNode *rhs_1 = make_node(AST_NODE_TERM, NULL, NULL, NULL, make_tok(TOKEN_INTEGER, NULL, 1, 0), TYPE_INT, false);
    ASTNode *add_then = make_node(AST_NODE_EXPR, rhs_zak, rhs_1, NULL, make_tok(TOKEN_PLUS, NULL, 0, 0), TYPE_UNKNOWN, false);
    ASTNode *assign_then = make_node(AST_NODE_ASSIGN, lhs_vysl, add_then, NULL, make_tok(TOKEN_ASSIGN, NULL, 0, 0), TYPE_UNKNOWN, false);

    // Link the variable declaration and assignment within the THEN branch.
    decl_zaklad_inner->next = assign_then;

    // 4. ELSE branch: create a while loop that runs while vysledok < 5.0.

    // Define the loop condition: vysledok < 5.0.
    ASTNode *w_cond_l = make_node(AST_NODE_TERM, NULL, NULL, NULL, make_tok(TOKEN_IDENTIFIER, "vysledok", 0, 0), TYPE_UNKNOWN, false);
    ASTNode *w_cond_r = make_node(AST_NODE_TERM, NULL, NULL, NULL, make_tok(TOKEN_DOUBLE, NULL, 0, 5.0), TYPE_FLOAT, false);
    ASTNode *w_cond = make_node(AST_NODE_EXPR, w_cond_l, w_cond_r, NULL, make_tok(TOKEN_LESS, NULL, 0, 0), TYPE_UNKNOWN, false);

    // Define the loop body: increment vysledok by 1.0.
    ASTNode *w_lhs = make_node(AST_NODE_TERM, NULL, NULL, NULL, make_tok(TOKEN_IDENTIFIER, "vysledok", 0, 0), TYPE_UNKNOWN, false);
    ASTNode *w_add_l = make_node(AST_NODE_TERM, NULL, NULL, NULL, make_tok(TOKEN_IDENTIFIER, "vysledok", 0, 0), TYPE_UNKNOWN, false);
    ASTNode *w_add_r = make_node(AST_NODE_TERM, NULL, NULL, NULL, make_tok(TOKEN_DOUBLE, NULL, 0, 1.0), TYPE_FLOAT, false);
    ASTNode *w_add = make_node(AST_NODE_EXPR, w_add_l, w_add_r, NULL, make_tok(TOKEN_PLUS, NULL, 0, 0), TYPE_UNKNOWN, false);
    ASTNode *w_assign = make_node(AST_NODE_ASSIGN, w_lhs, w_add, NULL, make_tok(TOKEN_ASSIGN, NULL, 0, 0), TYPE_UNKNOWN, false);
    ASTNode *while_node = make_node(AST_NODE_WHILE, w_cond, w_assign, NULL, make_tok(TOKEN_WHILE, NULL, 0, 0), TYPE_UNKNOWN, false);

    // Define the IF condition in main: vysledok > 10.
    ASTNode *if_cond_l = make_node(AST_NODE_TERM, NULL, NULL, NULL, make_tok(TOKEN_IDENTIFIER, "vysledok", 0, 0), TYPE_UNKNOWN, false);
    ASTNode *if_cond_r = make_node(AST_NODE_TERM, NULL, NULL, NULL, make_tok(TOKEN_INTEGER, NULL, 10, 0), TYPE_INT, false);
    ASTNode *if_cond = make_node(AST_NODE_EXPR, if_cond_l, if_cond_r, NULL, make_tok(TOKEN_GREATER, NULL, 0, 0), TYPE_UNKNOWN, false);

    // Create the IF statement with the variable declaration in the THEN branch
    // and the while loop in the ELSE branch.
    ASTNode *if_main = make_node(AST_NODE_IF, if_cond, decl_zaklad_inner, while_node, make_tok(TOKEN_IF, NULL, 0, 0), TYPE_UNKNOWN, false);

    // 5. Call print(vysledok) to output the final result.
    ASTNode *print_arg = make_node(AST_NODE_TERM, NULL, NULL, NULL, make_tok(TOKEN_IDENTIFIER, "vysledok", 0, 0), TYPE_UNKNOWN, false);
    ASTNode *call_print = make_node(AST_NODE_CALL, NULL, print_arg, NULL, make_tok(TOKEN_IDENTIFIER, "print", 0, 0), TYPE_UNKNOWN, false);

    // Link all statements in the main function using the 'next' pointer.
    decl_zaklad->next = decl_vysledok;
    decl_vysledok->next = if_main;
    if_main->next = call_print;

    // Create the main function node and mark it as the program entry point.
    ASTNode *func_main = make_node(AST_NODE_FUNCTION, NULL, decl_zaklad, NULL, make_tok(TOKEN_IDENTIFIER, "main", 0, 0), TYPE_VOID, false);
    func_main->is_main = true;

    // Link the main function with the additional function definition.
    func_main->next = func_vypocitaj;

    // Create the root node representing the entire program.
    ASTNode *program = make_node(AST_NODE_PROGRAM, func_main, NULL, NULL, make_tok(TOKEN_EOF, NULL, 0, 0), TYPE_UNKNOWN, false);
    return program;
}

// freeing AST nodes recursively
void free_ast(ASTNode *node) {
    if (node == NULL) {
        return;
    }

    free_ast(node->left);
    free_ast(node->right);
    free_ast(node->extra);
    free_ast(node->next);

    if (node->val.type == TOKEN_IDENTIFIER || node->val.type == TOKEN_STRING) {
        free(node->val.attribute.string_value);
    }

    free(node);
}

// Custom error handler for reporting semantic analysis errors and freeing resources.
void dominik_error(int code, char *msg) {
    fprintf(stderr, "Semantic Error %d: %s\n", code, msg);

    if (g_st != NULL) {
        symtable_dispose(g_st);
        g_st = NULL;
    }
    if (g_root != NULL) {
        free_ast(g_root);
        g_root = NULL;
    }

    exit(code);
}

int main(void) {
    // Initialize the symbol table used during semantic analysis.
    g_st = symtable_init();

    // Build a manually constructed AST for testing purposes.
    g_root = build_test_ast();

    // Run semantic analysis.
    bool ok = check_semantics(g_root, g_st);

    // Report the result if semantic analysis completes successfully.
    if (ok) {
        printf("Semantic analysis completed SUCCESSFULLY!\n");
    }

    // Release the allocated symbol table resources.
    symtable_dispose(g_st);
    g_st = NULL;

    // Release the entire AST (including mangled strings and inserted INT2FLOAT nodes).
    free_ast(g_root);
    g_root = NULL;

    return 0;
}