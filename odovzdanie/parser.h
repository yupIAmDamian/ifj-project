// Compiler of IFJ26 imperative language
// Author: Michal Kopček (xkopcem00)

#ifndef PARSER_H
#define PARSER_H
#include "scanner.h"
#include "ast.h"
#include "expression.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

extern Token current_token;

/**
 * @brief Retrieves the next token from the lookahead buffer or from the
 * scanner.
 * @return Next Token from the input stream.
 */
Token get_token_buffered(void);

/**
 * @brief Pushes a single token back into the one-token lookahead buffer.
 * @param t Token to be returned on the next get_token_buffered() call.
 */
void unget_token(Token t);

/**
 * @brief Checks if the current token matches the expected type and advances to
 * the next token if it does.
 * @param expected_type Expected TokenType to match against current_token.
 * @return true if the token matched and parser advanced, false otherwise.
 */
bool match(TokenType expected_type);

/**
 * @brief Entry point of syntactic analysis; initializes the scanner and parses
 * the entire program into global_ast_root.
 * @return 0 on success, 1 on lexical error, or 2 on syntax error.
 */
int parse();

/**
 * @brief Parses a function call including its identifier and argument list.
 * @return Pointer to the created AST_NODE_CALL node, or NULL on syntax error.
 */
ASTNode *parse_func_call();

/**
 * @brief Parses the top-level program structure (prolog followed by function
 * definitions until EOF).
 * @return Pointer to the root AST_NODE_PROGRAM node, or NULL on syntax error.
 */
ASTNode *parse_program();

/**
 * @brief Parses and validates the mandatory program prolog ('import ifj26.*'
 * followed by EOL).
 * @return true if the prolog is valid, false on syntax error.
 */
bool parse_prolog();

/**
 * @brief Parses all top-level function definitions and links them into a doubly
 * linked list.
 * @param out_functions Output pointer set to the first function node in the
 * list.
 * @return true if all functions were parsed successfully, false on syntax
 * error.
 */
bool parse_functions(ASTNode **out_functions);

/**
 * @brief Parses a single function definition including optional '@main'
 * annotation, parameters, return type, and body.
 * @return Pointer to the created AST_NODE_FUNCTION node, or NULL on syntax
 * error.
 */
ASTNode *parse_function();

/**
 * @brief Parses an optional '@main' annotation preceding a function definition.
 * @param is_main Output flag set to true if '@main' is present, false
 * otherwise.
 * @return true if the annotation is valid or absent, false on syntax error.
 */
bool parse_main_annotation(bool *is_main);

/**
 * @brief Parses an optional comma-separated list of formal function parameters.
 * @return Pointer to the first AST_NODE_PARAM node, or NULL if there are no
 * parameters or on syntax error.
 */
ASTNode *parse_params_opt();

/**
 * @brief Parses a single formal parameter in the form 'id: Type'.
 * @return Pointer to the created AST_NODE_PARAM node, or NULL on syntax error.
 */
ASTNode *parse_param();

/**
 * @brief Parses an optional function return type annotation (': Type').
 * @param out Output pointer storing the parsed DataType, or TYPE_UNKNOWN if
 * omitted.
 * @return true if valid or omitted, false on syntax error.
 */
bool parse_return_type_opt(DataType *out);

/**
 * @brief Parses a built-in IFJ26 data type keyword (Int, Double, String, or
 * Unit).
 * @param out Output pointer storing the corresponding DataType.
 * @return true if a valid type keyword was matched, false otherwise.
 */
bool parse_type(DataType *out);

/**
 * @brief Parses a function body, either as an indented block of statements or a
 * single inline statement.
 * @return Pointer to the first statement AST node of the body, or NULL on
 * syntax error.
 */
ASTNode *parse_function_body();

/**
 * @brief Parses a sequence of statements inside an indented block until DEDENT
 * or EOF.
 * @return Pointer to the head of the doubly linked statement AST nodes, or NULL
 * on syntax error.
 */
ASTNode *parse_statements();

/**
 * @brief Parses a single statement (return, while, if-else, val/var
 * declaration, assignment, or expression/call).
 * @return Pointer to the created statement AST node, or NULL on syntax error.
 */
ASTNode *parse_statement();

/**
 * @brief Parses an if statement, including optional 'else if' chains and
 * optional 'else' block.
 * @return Pointer to the created AST_NODE_IF node, or NULL on syntax error.
 */
ASTNode *parse_if_statement();

/**
 * @brief Parses an optional variable type annotation (': Type').
 * @param out Output pointer storing the parsed DataType, or TYPE_UNKNOWN if
 * omitted.
 * @return true if valid or omitted, false on syntax error.
 */
bool parse_type_opt(DataType *out);

/**
 * @brief Parses an expression or function call.
 * @return Pointer to the resulting AST node, or NULL on syntax error.
 */
ASTNode *parse_expr_or_call();

/**
 * @brief Parses an optional expression or function call (e.g., in return
 * statements).
 * @return Pointer to the resulting AST node, or NULL if omitted or on syntax
 * error.
 */
ASTNode *parse_expr_or_call_opt();

/**
 * @brief Parses an optional comma-separated list of function call arguments.
 * @return Pointer to the first argument AST node, or NULL if empty or on syntax
 * error.
 */
ASTNode *parse_args_opt();

/**
 * @brief Parses a single literal, identifier term, or unit literal '()'.
 * @return Pointer to the created AST_NODE_TERM or AST_NODE_UNIT node, or NULL
 * on syntax error.
 */
ASTNode *parse_term();

/**
 * @brief Frees all allocated AST nodes via ast_cleanup() and resets
 * global_ast_root to NULL.
 */
void parser_cleanup(void);

#endif