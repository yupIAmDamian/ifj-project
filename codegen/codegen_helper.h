// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)
#ifndef CODEGEN_HELPER_H
#define CODEGEN_HELPER_H
#include "../syntactic_analysis/ast.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Global variable to store the name of the current function.
 * @note This variable should be set before generating code for a function.
 */
extern char *current_func_name;

/*
 * @brief Global counter for generating unique labels.
 * @note This counter is incremented each time a new label is created.
 */
extern int label_counter;


/**
 * @brief Retrieves a pointer to the Token stored inside an AST node.
 * @param node Pointer to the AST node.
 * @return Pointer to the node's Token structure.
 */
Token* get_token_from_node(ASTNode *node);

/**
 * @brief Extracts the string attribute value from an AST node's token.
 * @param node Pointer to the AST node containing a string token.
 * @return String value from the token's attribute.
 */
char* gen_string_from_node(ASTNode *node);

/**
 * @brief Prints a string formatted for IFJcode26, escaping whitespace, backslashes, hashes, and non-printable ASCII characters as \XYZ.
 * @param value String to escape and print to stdout.
 */
void gen_escaped_string(char *value);

/**
 * @brief Dynamically allocates and formats a unique IFJcode26 label in the form "$<func>$<name>$<number>$<type>".
 * @param number Unique numeric identifier for the label.
 * @param label_name Base descriptive name of the label ( "if", "while").
 * @param label_type Specific role or branch of the label ("else", "end").
 * @return Pointer to the newly allocated label string, or NULL if formatting or allocation fails.
 */
char *create_label(int number, char* label_name, char* label_type);

/*
 * @brief Checks if the current function is the main function.
 * @return True if the current function is "main", false otherwise.
*/
bool in_main();

/**
 * @brief Generates a fresh unique ID.
 * @return A unique integer ID.
 */
int fresh_id();

/**
 * @brief Returns the name of a scratch variable based on the current function.
 * @return A string representing the scratch variable.
 */
char *scratch();

/*
 * @brief Checks if a code block always returns a value.
 * @param node The root of the code block to check.
 * @return True if the code block always returns a value, false otherwise.
*/
bool always_returns(ASTNode *node);


/*
* @brief Returns the name of a temporary variable based on the current function and index.
* @param i Index to determine which temporary variable to return (0 or 1).
* @return A string representing the temporary variable.
*/
char *concat_tmp(int i);


/*
 * @brief Checks if an AST node represents a float operand.
 * @param node The AST node to check.
 * @return True if the node is a float operand, false otherwise.
 */
bool is_float_operand(ASTNode *node);


#endif