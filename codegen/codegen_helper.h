#ifndef CODEGEN_HELPER_H
#define CODEGEN_HELPER_H
#include "../syntactic_analysis/ast.h"
#include <stdio.h>
#include <stdlib.h>

Token* get_token_from_node(ASTNode *node);

char* gen_string_from_node(ASTNode *node);

void gen_escaped_string(char *value);


extern char *current_func_name;

/** 
 * Creates a label for an else clause in the generated code.
 * @param number The number of the else clause.
 * @return A pointer to the created label, or NULL if allocation fails.
 */
//Needs to be freed
char *create_label(int number, char* label_name, char* label_type);

#endif