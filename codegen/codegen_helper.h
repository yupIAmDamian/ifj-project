// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)

#ifndef CODEGEN_HELPER_H
#define CODEGEN_HELPER_H
#include "../syntactic_analysis/ast.h"
#include <stdio.h>
#include <stdlib.h>

extern char *current_func_name;

/**
 * @brief Allocates and initializes an AST conversion node (Int -> Float).
 * @param expr Pointer to the integer expression AST node to be wrapped.
 * @return Pointer to the newly created AST_NODE_INT2FLOAT node, or NULL if expr is NULL.
 */
Token* get_token_from_node(ASTNode *node);

/**
 * @brief Allocates and initializes an AST conversion node (Int -> Float).
 * @param expr Pointer to the integer expression AST node to be wrapped.
 * @return Pointer to the newly created AST_NODE_INT2FLOAT node, or NULL if expr is NULL.
 */
char* gen_string_from_node(ASTNode *node);

/**
 * @brief Allocates and initializes an AST conversion node (Int -> Float).
 * @param expr Pointer to the integer expression AST node to be wrapped.
 * @return Pointer to the newly created AST_NODE_INT2FLOAT node, or NULL if expr is NULL.
 */
void gen_escaped_string(char *value);



/**
 * @brief creates new label according to type of control statement.
 * @param int Pointer to the integer expression AST node to be wrapped.
 * @param char* Pointer to the name of control statement type .
 * @param char* Pointer to the integer expression AST node to be wrapped.
 * @return Pointer to the newly created char.
 * @note pointer to newly created string should be freed. 
 */
char *create_label(int number, char* label_name, char* label_type);

#endif