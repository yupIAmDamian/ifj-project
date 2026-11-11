#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "ast.h"
#include "scanner.h"
#include <string.h>
#include "parser.h"

#include "gen_func.h"
#include "codegen_helper.h"

char* current_func_name;

void gen_code(ASTNode *root);
void gen_node(ASTNode *node);


/*
Function definition
*/
void gen_fun_def_start();

void gen_fun_def_args(ASTNode *root, char *function_name);

void gen_fun_def_vars(ASTNode *root, int *var_counter, char *function_name);

void gen_fun_def_body(ASTNode *root, char *function_name);

void gen_fun_def_end();

void gen_fun_call(ASTNode *node, char *result_var);

void gen_if(ASTNode *node);
void decide_condition(ASTNode *condition_node);
void gen_while(ASTNode *node);

void gen_data_type(ASTNode *node);
void gen_pushs(char *symb);
void gen_pushs_data_types(Token *symb);

void gen_adds();
void gen_subs();
void gen_muls();
void gen_divs();

#endif