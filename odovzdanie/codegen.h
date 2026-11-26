// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)
#ifndef CODEGEN_H
#define CODEGEN_H

#include "scanner.h"
#include "ast.h"
#include "parser.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "helper_func.h"
#include "codegen_helper.h"
#include "gen_func.h"

/**
- @brief Traverses a linked list of AST nodes and generates target IFJcode26
instructions for each.
- @param root Pointer to the first AST node in the sequence.
 */
void gen_code(ASTNode *root);

/**
- @brief Dispatches code generation based on the specific type of an AST node.
- @param node Pointer to the AST node to be processed.
 */
void gen_node(ASTNode *node);

/*
Function definition
*/
/**
- @brief Generates the opening label and frame setup for a function definition.
- @note Uses global variables if in main; otherwise creates and pushes a new
local frame.
 */
void gen_fun_def_start();

/**
- @brief Generates parameter declarations for a function definition.
- @param root Pointer to the first parameter node in the AST parameter list.
 */
void gen_fun_def_args(ASTNode *root);

/**
- @brief Pops passed argument values from the data stack into function
parameters.
- @param node Pointer to the first parameter node in the AST parameter list.
 */
void gen_fun_init_args(ASTNode *node);

/**
- @brief Recursively traverses a function's AST to declare all local variables
at the start of the frame.
- @param node Pointer to the root AST node of the function body.
 */
void gen_fun_def_vars_recursive(ASTNode *node);

/**
- @brief Generates target instructions for all statements within a function's
body.
- @param root Pointer to the first statement node in the function body.
 */
void gen_fun_def_body(ASTNode *root);

/**
- @brief Generates the cleanup and return instructions at the end of a function.
- @note Emits EXIT int@0 if in main; otherwise pops the frame and emits RETURN.
 */
void gen_fun_def_end();

/**
- @brief Generates comparison instructions for an operand, converting it to a
float if necessary.
- @param node Pointer to the AST node representing the operand.
- @param want_float True if the operand should be converted to a float, false
otherwise.
 */
void gen_compare_operand(ASTNode *node, bool want_float);

/**
- @brief Evaluates a boolean condition AST subtree and leaves the boolean result
on top of the data stack.
- @param condition_node Pointer to the root AST node of the condition
expression.
 */
void decide_condition(ASTNode *condition_node);

// If statement
/**
- @brief Generates conditional jump instructions for an if statement, including
then and else branches.
- @param node Pointer to the AST_NODE_IF node.
 */
void gen_if(ASTNode *node);

/**
- @brief Generates condition evaluation, conditional jump to an else/end label,
and the body block.
- @param condition_node Pointer to the condition expression AST node.
- @param then_body Pointer to the AST node representing the body executed when
the condition holds.
- @param else_label Target label name to jump to if the condition evaluates to
false.
 */
void gen_condition_statement_while_if(ASTNode *condition_node,
                                      ASTNode *then_body, char *else_label);

/**
- @brief Generates loop labels, condition checks, and body instructions for a
while statement.
- @param node Pointer to the AST_NODE_WHILE node.
 */
void gen_while(ASTNode *node);

/**
- @brief Recursively traverses an arithmetic or string expression AST and
generates stack-based operations.
- @param node Pointer to the root AST node of the expression.
- @param result_var Optional name of the destination variable for the result.
 */
void equation_traversal(ASTNode *node, char *result_var);

/**
- @brief Generates argument pushes and the CALL instruction for user-defined or
built-in functions.
- @param node Pointer to the AST_NODE_CALL node.
 */
void gen_fun_call(ASTNode *node);

/**
- @brief Generates short-circuit evaluation logic for logical AND and OR
operators.
- @param node Pointer to the logical operator AST node.
- @param is_and True if the operator is logical AND, false if logical OR.
 */
void gen_short_circuit(ASTNode *node, bool is_and);

/**
- @brief Generates instructions to push a single operand onto the data stack.
- @param node Pointer to the operand AST node.
 */
void gen_operand(ASTNode *node);

/**
- @brief Evaluates an  value node (variable, literal, call, or expression) and
pushes it onto the data stack.
- @param node Pointer to the AST node representing the value.
 */
void gen_push_value(ASTNode *node);
#endif