#ifndef AST_H
#define AST_H
#include "scanner.h"

typedef enum {
    AST_NODE_PROGRAM,       // root node for the program
    AST_NODE_FUNCTION,      // function definition
    AST_NODE_VAR_DECL,      // variable declaration
    AST_NODE_IF,            // if statement
    AST_NODE_WHILE,         // while loop
    AST_NODE_RETURN,        // return statement
    AST_NODE_ADD,           // +
    AST_NODE_SUB,           // -
    AST_NODE_MUL,           // *
    AST_NODE_DIV,           // /
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    struct ASTNode *left;  // left child
    struct ASTNode *right; // right child
    struct ASTNode *extra; // extra child for statements like if and while
    struct ASTNode *next;  // next node in the list
    Token val;
} ASTNode;

ASTNode *create_ast_node(ASTNodeType type,ASTNode *left, ASTNode *right, ASTNode *extra, Token val);

#endif