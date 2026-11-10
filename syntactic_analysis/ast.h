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
    AST_NODE_EXPR,          // expression node
    AST_NODE_PARAM,         // function parameter
    AST_NODE_VAR,           // variable node
    AST_NODE_TERM,          // term node (identifier, integer, double, string)
    AST_NODE_ASSIGN,        // assignment node
    AST_NODE_CALL,          // function call node
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *extra;
    struct ASTNode *next;
    Token val;
    bool is_main;
} ASTNode;

ASTNode *create_ast_node(ASTNodeType type,ASTNode *left, ASTNode *right, ASTNode *extra, Token val);

#endif