// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)

#ifndef GEN_FUNC_H
#define GEN_FUNC_H

#include <stddef.h>
#include <stdio.h>
#include "../syntactic_analysis/ast.h"


extern char *current_func_name; 

void gen_params_declaration(char *var_name);

void create_head();

void gen_new_line();

void gen_defvar();
void gen_move();
void gen_createframe();
void gen_popframe();
void gen_return();
void gen_pushframe();
void gen_pops(char *var);
void gen_clears();
void gen_break();

void gen_adds();
void gen_subs();
void gen_muls();
void gen_divs();
void gen_idivs();
void gen_LTs();
void gen_GTs();
void gen_EQs();
void gen_ANDs();
void gen_ORs();
void gen_NOTs();
void gen_int2floats();
void gen_float2ints();
void gen_int2chars();
void gen_stri2ints();
void gen_float2strs();
void gen_int2strs();
void gen_types();
void gen_isints();

void gen_jumpifeqs(char *label);
void gen_jumpifneqs(char *label);

void gen_dprint(char *symb);
void gen_read();
void gen_write(ASTNode *node);
void gen_read_default(char *var, char* data_type, char *default_val);
void gen_concat(char *var, char *symb1, char *symb2);
void gen_strlen();
void gen_getchar(char *var, char *symb1, char *symb2);
void gen_setchar(char *var, char *symb1, char *symb2);
void gen_pushs(ASTNode *node);
void gen_argument(ASTNode *arg_node);
void gen_data_type(ASTNode *node);


void gen_label(char *label);
void gen_jump(char *label);
void gen_exit();

void gen_builtin_ord();
void gen_builtin_substr();

#endif