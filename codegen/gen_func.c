#include "gen_func.h"
#include "scanner.h"

//function declaration
void gen_params_declaration(char *scope, char* func_name, char* var_name){
    gen_defvar();
    gen_var(scope, func_name, var_name, NULL);
    gen_new_line();
    gen_pops();
    gen_var(scope, func_name, var_name, NULL);
    gen_new_line();
}

//variable generation
void gen_var(char *scope, char* func_name, char* var_name, int number){
    printf("%s@%s$%s%d", scope, func_name, var_name, number);
}

//rest code
void create_head(){
    printf(".IFJcode26\n");
    printf("JUMP $main\n\n");
}

void gen_new_line(){
    printf("\n");
}

void gen_defvar(){
    printf("DEFVAR");
}

void gen_move(){
    printf("MOVE");
}

void gen_createframe(){
    printf("CREATEFRAME\n");
}

void gen_popframe(){
    printf("POPFRAME\n");
}

void gen_return(){
    printf("RETURN\n");
}

void gen_pushframe(){
    printf("PUSHFRAME\n");
}

void gen_pops(){
    printf("POPS");
}

void gen_clears(){
    printf("CLEARS\n");
}

void gen_break(){
    printf("BREAK\n");
}

void gen_adds(){
    printf("ADDS\n");
}

void gen_subs(){
    printf("SUBS\n");
}

void gen_muls(){
    printf("MULS\n");
}

void gen_divs(){
    printf("DIVS\n");
}

void gen_idivs(){
    printf("IDIVS\n");
}

void gen_LTs(){
    printf("LTS\n");
}

void gen_GTs(){
    printf("GTS\n");
}

void gen_EQs(){
    printf("EQS\n");
}

void gen_ANDs(){
    printf("ANDS\n");
}

void gen_ORs(){
    printf("ORS\n");
}

void gen_NOTs(){
    printf("NOTS\n");
}

void gen_int2floats(){
    printf("INT2FLOATS\n");
}

void gen_float2ints(){
    printf("FLOAT2INTS\n");
}

void gen_int2chars(){
    printf("INT2CHARS\n");
}

void gen_stri2ints(){
    printf("STRI2INTS\n");
}

void gen_float2strs(){
    printf("FLOAT2STRS\n");
}

void gen_int2strs(){
    printf("INT2STRS\n");
}

void gen_types(){
    printf("TYPES\n");
}

void gen_isints(){
    printf("ISINTS\n");
}


/*
Functions where something needs to be passed as an argument, like a label or a variable name
*/
//label
void gen_jumpifeqs(char *label){
    printf("JUMPIFEQS %s\n", label);
}

//label
//TODO maybbe this is not because i can do it with condition checking and jumpifeqs
void gen_jumpifneqs(char *label){
    printf("JUMPIFNEQS %s\n", label);
}

//what to print
void gen_dprint(){
    printf("DPRINT");
}

//variable name + data type
void gen_read(){
    printf("READ");
}

void gen_write(char *symb){
    //TODO: musim tu riesit aj datovy typ, lebo sa to bude lisit podla toho co sa bude vypisovat
    printf("WRITE %s\n", symb);
}

void gen_concat(char *var, char *symb1, char *symb2){
    printf("CONCAT %s %s %s\n", var, symb1, symb2);
}

void gen_strlen(char *var, char *symb){
    printf("STRLEN %s %s\n", var, symb);
}

void gen_getchar(char *var, char *symb1, char *symb2){
    printf("GETCHAR %s %s %s\n", var, symb1, symb2);
}

void gen_setchar(char *var, char *symb1, char *symb2){
    printf("SETCHAR %s %s %s\n", var, symb1, symb2);
}

void gen_newarray(char *var, char *symbs){
    printf("NEWARRAY %s %s\n", var, symbs);
}

void gen_arrayget(char *var, char *var_a, char *symb_index){
    printf("ARRAYGET %s %s %s\n", var, var_a, symb_index);
}

void gen_arrayset(char *var_a, char *symb_index, char *symb_val){
    printf("ARRAYSET %s %s %s\n", var_a, symb_index, symb_val);
}

void gen_label(char *label){
    printf("LABEL %s\n", label);
}

void gen_jump(){
    printf("JUMP");
}

void gen_exit(){
    printf("EXIT");
}

//label
void gen_call(){
    printf("CALL");
}