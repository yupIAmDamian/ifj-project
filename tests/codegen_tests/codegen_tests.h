// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)

#ifndef CODEGEN_TESTS_H
#define CODEGEN_TESTS_H

#include "../../codegen/codegen.h"
#include "../../syntactic_analysis/ast.h"

/**
 * @brief Creates a basic Token initialized with a given type and line number.
 * @param type TokenType enumeration value for the token.
 * @param line Source code line number associated with the token.
 * @return Initialized Token structure.
 */
Token make_token(TokenType type, int line);

/**
 * @brief Creates an identifier Token with the specified name and line number.
 * @param name String representing the identifier name.
 * @param line Source code line number associated with the token.
 * @return Initialized identifier Token structure.
 */
Token identifier_token(const char *name, int line);

/**
 * @brief Creates an integer Token with the specified value and line number.
 * @param value Integer value.
 * @param line Source code line number associated with the token.
 * @return Initialized integer Token structure.
 */
Token integer_token(int value, int line);

/**
 * @brief Creates a string Token with the specified value and line number.
 * @param value String value.
 * @param line Source code line number associated with the token.
 * @return Initialized string Token structure.
 */
Token string_token(const char *value, int line);

/**
 * @brief Wraps a Token into a leaf expression AST node.
 * @param token Token representing the expression value.
 * @return Pointer to the newly created AST_NODE_EXPR node.
 */
ASTNode *expression_value(Token token);

/**
 * @brief Creates an expression AST node representing a variable identifier.
 * @param name Identifier name.
 * @param line Source code line number.
 * @return Pointer to the newly created AST_NODE_EXPR node.
 */
ASTNode *identifier_expression(const char *name, int line);

/**
 * @brief Creates an expression AST node representing an integer constant.
 * @param value Integer constant value.
 * @param line Source code line number.
 * @return Pointer to the newly created AST_NODE_EXPR node.
 */
ASTNode *integer_expression(int value, int line);

/**
 * @brief Wraps a Token into a term AST node.
 * @param token Token representing the term.
 * @return Pointer to the newly created AST_NODE_TERM node.
 */
ASTNode *term(Token token);

/**
 * @brief Creates a term AST node representing an identifier.
 * @param name Identifier name.
 * @param line Source code line number.
 * @return Pointer to the newly created AST_NODE_TERM node.
 */
ASTNode *term_identifier(const char *name, int line);

/**
 * @brief Creates a term AST node representing a string.
 * @param value String value.
 * @param line Source code line number.
 * @return Pointer to the newly created AST_NODE_TERM node.
 */
ASTNode *term_string(const char *value, int line);

/**
 * @brief Creates a binary operation expression AST node with left and right operand subtrees.
 * @param operator_type TokenType representing the binary operator.
 * @param left Pointer to the left operand AST node.
 * @param right Pointer to the right operand AST node.
 * @param line Source code line number.
 * @return Pointer to the newly created binary AST_NODE_EXPR node.
 */
ASTNode *binary_expression(TokenType operator_type,
                                  ASTNode *left, ASTNode *right, int line);

/**
 * @brief Creates a function call AST node with the given function name and argument list.
 * @param name Name of the function being called.
 * @param arguments Pointer to the head of the linked list of argument AST nodes.
 * @param line Source code line number.
 * @return Pointer to the newly created AST_NODE_CALL node.
 */
ASTNode *call(const char *name, ASTNode *arguments, int line);

/**
 * @brief Creates a variable reference AST node.
 * @param name Variable identifier name.
 * @param line Source code line number.
 * @return Pointer to the newly created AST_NODE_VAR node.
 */
ASTNode *variable(const char *name, int line);

/**
 * @brief Creates a variable or constant declaration AST node with an initializer expression.
 * @param name Variable identifier name.
 * @param initializer Pointer to the AST node representing the initial value expression.
 * @param is_const Flag indicating whether the declaration is immutable (val) or mutable (var).
 * @param line Source code line number.
 * @return Pointer to the newly created AST_NODE_VAR_DECL node.
 */
ASTNode *declaration(const char *name, ASTNode *initializer,
                            bool is_const, int line);

/**
 * @brief Creates a variable assignment AST node.
 * @param name Variable identifier name.
 * @param value Pointer to the AST node representing the assigned value expression.
 * @param line Source code line number.
 * @return Pointer to the newly created AST_NODE_ASSIGN node.
 */
ASTNode *assignment(const char *name, ASTNode *value, int line);

/**
 * @brief Creates a return statement AST node.
 * @param value Pointer to the return value expression AST node, or NULL for void returns.
 * @param line Source code line number.
 * @return Pointer to the newly created AST_NODE_RETURN node.
 */
ASTNode *return_statement(ASTNode *value, int line);

/**
 * @brief Appends an AST node to the end of a doubly-linked list of sibling nodes.
 * @param first Double pointer to the head of the AST node list.
 * @param last Double pointer to the tail of the AST node list.
 * @param node Pointer to the AST node to append.
 */
void append_node(ASTNode **first, ASTNode **last, ASTNode *node);

/**
 * @brief Creates a function parameter AST node.
 * @param name Variable identifier name.
 * @param line Source code line number.
 * @return Pointer to the newly created AST_NODE_PARAM node.
 */
ASTNode *parameter(const char *name, int line);

/**
 * @brief Creates a conditional if-else statement AST node.
 * @param condition Pointer to the condition expression AST node.
 * @param then_body Pointer to the AST node list executed when the condition is true.
 * @param else_body Pointer to the AST node list executed when the condition is false.
 * @param line Source code line number.
 * @return Pointer to the newly created AST_NODE_IF node.
 */
ASTNode *if_node(ASTNode *condition, ASTNode *then_body,
                        ASTNode *else_body, int line);

/**
 * @brief Creates a while loop statement AST node.
 * @param condition Pointer to the loop condition expression AST node.
 * @param body Pointer to the AST node list representing the loop body.
 * @param line Source code line number.
 * @return Pointer to the newly created AST_NODE_WHILE node.
 */
ASTNode *while_node(ASTNode *condition, ASTNode *body, int line);

/**
 * @brief Creates a function definition AST node.
 * @param name Function name.
 * @param parameters Pointer to the head of the parameter AST node list.
 * @param body Pointer to the head of the function body statement AST node list.
 * @param is_main Flag indicating whether this function is the @main entry point.
 * @param line Source code line number.
 * @return Pointer to the newly created AST_NODE_FUNCTION node.
 */
ASTNode *function(const char *name, ASTNode *parameters,
                         ASTNode *body, bool is_main, int line);

/**
 * @brief Constructs a complete mock AST for a program computing factorials iteratively and recursively.
 * @return Pointer to the root AST_NODE_PROGRAM node of the constructed tree.
 */
ASTNode *create_factorial_tree(void);

/**
 * @brief Entry point that constructs the factorial AST and runs code generation on it.
 * @return 0 upon successful execution.
 */
int main();

#endif
