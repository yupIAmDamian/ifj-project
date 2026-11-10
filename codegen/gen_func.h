#ifndef GEN_FUNC_H
#define GEN_FUNC_H

#include <stddef.h>
#include <stdio.h>

void gen_params_declaration(char *scope, char* func_name, char* var_name);

void gen_var(char *scope, char* func_name, char* var_name, int number);

void create_head();

void gen_new_line();

void gen_defvar();
void gen_move();
void gen_createframe();
void gen_popframe();
void gen_return();
void gen_pushframe();
void gen_pops();
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

void gen_dprint();
void gen_read();
void gen_write(char *symb);
void gen_concat(char *var, char *symb1, char *symb2);
void gen_strlen(char *var, char *symb);
void gen_getchar(char *var, char *symb1, char *symb2);
void gen_setchar(char *var, char *symb1, char *symb2);

//TODO: do we need them or not? bacause we are not really generating code for arrays
void gen_newarray(char *var, char *symbs);
void gen_arrayget(char *var, char *var_a, char *symb_index);
void gen_arrayset(char *var_a, char *symb_index, char *symb_val);

void gen_label(char *label);
void gen_jump();
void gen_exit();
void gen_call();

#endif