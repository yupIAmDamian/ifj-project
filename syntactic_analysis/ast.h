// Compiler of IFJ26 imperative language
// Author: Michal Kopček (xkopcem00)

#ifndef AST_H
#define AST_H
#include "../error.h"
#include "../hashtable/symtable.h"
#include "../scanner/scanner.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
  AST_NODE_PROGRAM,   // root node for the program
  AST_NODE_FUNCTION,  // function definition
  AST_NODE_VAR_DECL,  // variable declaration
  AST_NODE_IF,        // if statement
  AST_NODE_WHILE,     // while loop
  AST_NODE_RETURN,    // return statement
  AST_NODE_ADD,       // +
  AST_NODE_SUB,       // -
  AST_NODE_MUL,       // *
  AST_NODE_DIV,       // /
  AST_NODE_EXPR,      // expression node
  AST_NODE_PARAM,     // function parameter
  AST_NODE_VAR,       // variable node
  AST_NODE_TERM,      // term node (identifier, integer, double, string)
  AST_NODE_ASSIGN,    // assignment node
  AST_NODE_CALL,      // function call node
  AST_NODE_INT2FLOAT, // integer to float conversion node
  AST_NODE_UNIT,      // unit node
  AST_NODE_AND,       // &&
  AST_NODE_OR,        // ||
  AST_NODE_NOT,       // !
  AST_NODE_TRUE,      // node true
  AST_NODE_FALSE      // node false
} ASTNodeType;

typedef struct ASTNode {
  ASTNodeType type;
  DataType inferred_type;
  struct ASTNode *left;
  struct ASTNode *right;
  struct ASTNode *extra;
  struct ASTNode *next;
  struct ASTNode *prev;
  struct ASTNode *alloc_next;
  Token val;
  bool is_main;
  bool is_const;
  bool uses_ord;
  bool uses_substr;
} ASTNode;

/**
 * @brief Allocates and initializes a new AST node and registers it in the
 * intrusive allocation list.
 * @param type Type of the AST node (ASTNodeType).
 * @param left Pointer to the left child AST node (or NULL).
 * @param right Pointer to the right child AST node (or NULL).
 * @param extra Pointer to an auxiliary child AST node, such as an else branch
 * (or NULL).
 * @param val Lexical token associated with the node.
 * @return Pointer to the newly allocated ASTNode (exits with ERR_INTERNAL on
 * allocation failure).
 */
ASTNode *create_ast_node(ASTNodeType type, ASTNode *left, ASTNode *right,
                         ASTNode *extra, Token val);

/**
 * @brief Frees all allocated AST nodes and their owned string attributes via
 * the intrusive allocation list.
 */
void ast_cleanup(void);

#endif