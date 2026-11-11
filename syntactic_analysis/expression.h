#ifndef EXPRESSION_H
#define EXPRESSION_H
#include <stdbool.h>
#include "scanner.h"
#include "ast.h"

typedef enum {
    PREC_UNARY,     // unary +, -
    PREC_MUL_DIV,   // *, /
    PREC_ADD_SUB,   // binary +, -
    PREC_REL,       // <, <=, >, >=
    PREC_EQ_NOT_EQ, // ==, !=
    PREC_LPAREN,    // (
    PREC_RPAREN,    // )
    PREC_OPERAND,   // id, int, double, string
    PREC_DOLLAR,    // $ 
} PrecTableIndex;

typedef enum {
    P_S, // shift  '<'
    P_R, // reduce '>'
    P_E, // equal  '='
    P_N, // none   ' ' (error)
} PrecRelation;

typedef enum {
    SYM_TERMINAL, // real token from scanner
    SYM_NONTERM,  // non-terminal symbol from grammar
    SYM_MARK,     // symbol used for precedence parsing
} StackItemType;

typedef struct {
    StackItemType type;
    PrecTableIndex prec_index;
    Token token; 
    ASTNode *ast_node;
    struct StackItem *next;
} StackItem;

typedef struct {
    StackItem *top;
} PrecStack;

void expr_stack_init(PrecStack *stack);
bool expr_stack_push(PrecStack *stack, StackItemType type, PrecTableIndex index, Token token, ASTNode *ast_node);
bool expr_stack_insert_mark(PrecStack *stack);
void expr_stack_pop(PrecStack *stack);
StackItem *expr_stack_top_terminal(PrecStack *stack);
void expr_stack_free(PrecStack *stack);
PrecTableIndex get_prec_index(TokenType token_type, PrecStack *stack);

ASTNode* parse_expression();

#endif