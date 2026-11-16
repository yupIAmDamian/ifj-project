// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)
#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../syntactic_analysis/ast.h"
#include "../scanner/scanner.h"
#include "../syntactic_analysis/parser.h"

#include <string.h>

#include "gen_func.h"
#include "codegen_helper.h"

extern char* current_func_name;

void gen_code(ASTNode *root);
void gen_node(ASTNode *node);


/*
Function definition
*/
void gen_fun_def_start();

void gen_fun_def_args(ASTNode *root);

//TODO change name of it
void gen_fun_init_args(ASTNode *node);

void gen_fun_def_body(ASTNode *root);

void gen_fun_def_end();

void gen_fun_call(ASTNode *node, char *result_var);


void gen_if(ASTNode *node);
bool always_returns(ASTNode *node);
void decide_condition(ASTNode *condition_node);
void gen_while(ASTNode *node);

void gen_adds();
void gen_subs();
void gen_muls();
void gen_divs();

void gen_fun_def_vars_recursive(ASTNode *node);

void equation_traversal(ASTNode *node, char *result_var);
void arithmetic_operation(ASTNode *node);
void gen_call(ASTNode *node, char *result_var);

#endif