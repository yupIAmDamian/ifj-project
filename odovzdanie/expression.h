// Compiler of IFJ26 imperative language
// Author: Michal Kopček (xkopcem00)

#ifndef EXPRESSION_H
#define EXPRESSION_H
#include "ast.h"
#include "error.h"
#include "parser.h"
#include "scanner.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
  PREC_UNARY,     // unary +, -, !
  PREC_MUL_DIV,   // *, /
  PREC_ADD_SUB,   // binary +, -
  PREC_REL,       // <, <=, >, >=
  PREC_EQ_NOT_EQ, // ==, !=
  PREC_AND,       // &&
  PREC_OR,        // ||
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

typedef struct StackItem {
  StackItemType type;
  PrecTableIndex prec_index;
  Token token;
  ASTNode *ast_node;
  struct StackItem *next;
} StackItem;

typedef struct {
  StackItem *top;
} PrecStack;

/**
 * @brief Initializes the precedence parsing stack to an empty state.
 * @param stack Pointer to the PrecStack structure to initialize.
 */
void expr_stack_init(PrecStack *stack);

/**
 * @brief Pushes a new terminal, non-terminal, or handle mark item onto the
 * precedence stack.
 * @param stack Pointer to the PrecStack structure.
 * @param type Stack item category (SYM_TERMINAL, SYM_NONTERM, or SYM_MARK).
 * @param index Precedence table index associated with the item.
 * @param token Lexical token stored in the stack item.
 * @param ast_node Pointer to the associated AST subtree (NULL for terminals and
 * marks).
 * @return true on successful push (exits with ERR_INTERNAL on allocation
 * failure).
 */
bool expr_stack_push(PrecStack *stack, StackItemType type, PrecTableIndex index,
                     Token token, ASTNode *ast_node);

/**
 * @brief Inserts a reduction handle mark (SYM_MARK) directly above the topmost
 * terminal on the stack.
 * @param stack Pointer to the PrecStack structure.
 * @return true on successful insertion (exits with ERR_INTERNAL on allocation
 * failure).
 */
bool expr_stack_insert_mark(PrecStack *stack);

/**
 * @brief Removes and frees the top item from the precedence stack.
 * @param stack Pointer to the PrecStack structure.
 */
void expr_stack_pop(PrecStack *stack);

/**
 * @brief Finds the topmost terminal item (SYM_TERMINAL) on the precedence
 * stack.
 * @param stack Pointer to the PrecStack structure.
 * @return Pointer to the topmost terminal StackItem, or NULL if none exists.
 */
StackItem *expr_stack_top_terminal(PrecStack *stack);

/**
 * @brief Pops and frees all remaining items from the precedence stack.
 * @param stack Pointer to the PrecStack structure.
 */
void expr_stack_free(PrecStack *stack);

/**
 * @brief Maps an incoming TokenType to its corresponding row/column index in
 * the precedence table.
 * @param token_type Type of the current lookahead token.
 * @param stack Pointer to the PrecStack used to distinguish unary vs. binary
 * '+' and '-'.
 * @return Corresponding PrecTableIndex value (or PREC_DOLLAR for
 * end-of-expression tokens).
 */
PrecTableIndex get_prec_index(TokenType token_type, PrecStack *stack);

/**
 * @brief Frees the currently active precedence stack during error recovery or
 * exit cleanup.
 */
void expr_stack_cleanup(void);

/**
 * @brief Parses an expression using bottom-up operator-precedence analysis and
 * builds its AST subtree.
 * @return Pointer to the root ASTNode of the parsed expression (exits with
 * ERR_SYNTAX on invalid syntax).
 */
ASTNode *parse_expression(void);

#endif