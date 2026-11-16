// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)

#include "gen_func.h"
#include "codegen_helper.h"
#include "scanner.h"
#include <string.h>



//function declaration
void gen_params_declaration(char *var_name){
    gen_defvar();
    gen_var(var_name);
    gen_new_line();
}

//variable generation
void gen_var(char* var_name){
    //TODO check if There is GL in name of the var if there is dont add the function name to it
    if(strncmp(var_name, "GF@", 3) == 0 ) {
        printf("%s", var_name);
        return;
    }
    printf("%s$%s", var_name, current_func_name);
}

//rest code
//Should it be $main or main?
void create_head(){
    printf(".IFJcode26\n");
    printf("JUMP main\n\n");
}

void gen_new_line(){
    printf("\n");
}

void gen_defvar(){
    printf("DEFVAR ");
}

void gen_move(char* var_name){
    printf("MOVE %s ", var_name);
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

void gen_pops(char *var){
    printf("POPS ");
    if(var != NULL){
        gen_var(var);
    }
    gen_new_line();
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
void gen_dprint(char *symb){
    printf("DPRINT %s\n", symb);
}

void gen_pushs(ASTNode *node){
    printf("PUSHS ");
    gen_argument(node);
    gen_new_line();
}

void gen_argument(ASTNode *arg_node){
    Token *token = get_token_from_node(arg_node);
    if (token->type == TOKEN_IDENTIFIER) {
        gen_var(gen_string_from_node(arg_node));
    } else {
        gen_data_type(arg_node);
    }
}

void gen_data_type(ASTNode *node){
    Token *token = get_token_from_node(node);
    switch (token->type) {
        case TOKEN_INTEGER:
            printf("int@%d", token->attribute.integer_value);
            break;
        //TODO chceck if float is correct as a
        case TOKEN_DOUBLE:
            printf("float@%a", token->attribute.double_value);
            break;
        case TOKEN_STRING:
            printf("string@");
            gen_escaped_string(token->attribute.string_value);
            break;
        default:
            break;
    }
}

void gen_write(ASTNode *node){
    printf("WRITE ");
    gen_argument(node);
    gen_new_line();
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

void gen_jump(char *label){
    printf("JUMP %s\n", label);
}

void gen_exit(){
    printf("EXIT ");
}

static int next_read_label_id = 0;

void gen_read_default(char *var, char* data_type, char *default_val) {
    int id = next_read_label_id++;
    gen_read(var, data_type);
    printf("JUMPIFNEQ $read$ok$%d %s nil@nil\n", id, var);
    printf("MOVE %s %s\n", var, default_val);
    printf("LABEL $read$ok$%d\n", id);
}

//variable name + data type
void gen_read(char *var, char *data_type){
    printf("READ ");
    gen_var(var);
    printf(" %s\n", data_type);
}


//TODO make them stack versions
void gen_builtin_ord() {
    printf("LABEL $ord\n");
    printf("DEFVAR LF@ord$value\n");
    printf("DEFVAR LF@ord$index\n");
    printf("DEFVAR LF@ord$length\n");
    printf("DEFVAR LF@ord$char\n");
    printf("DEFVAR LF@ord$result\n");
    printf("DEFVAR LF@ord$invalid\n");

    printf("POPS LF@ord$index\n");
    printf("POPS LF@ord$value\n");

    printf("STRLEN LF@ord$length LF@ord$value\n");

    // invalid if index < 0
    printf("LT LF@ord$invalid LF@ord$index int@0\n");

    // invalid if index >= length
    printf("GT LF@ord$result LF@ord$index LF@ord$length\n");
    printf("EQ LF@ord$char LF@ord$index LF@ord$length\n");
    printf("OR LF@ord$result LF@ord$result LF@ord$char\n");
    printf("OR LF@ord$invalid LF@ord$invalid LF@ord$result\n");

    printf("JUMPIFEQ $ord$invalid LF@ord$invalid bool@true\n");

    printf("GETCHAR LF@ord$char LF@ord$value LF@ord$index\n");
    printf("STRI2INT LF@ord$result LF@ord$char int@0\n");
    printf("PUSHS LF@ord$result\n");
    printf("RETURN\n");

    printf("LABEL $ord$invalid\n");
    printf("PUSHS int@-1\n");
    printf("RETURN\n");
}

void gen_builtin_substr() {
    printf("LABEL $subStr\n");
    printf("DEFVAR LF@substr$value\n");
    printf("DEFVAR LF@substr$start\n");
    printf("DEFVAR LF@substr$end\n");
    printf("DEFVAR LF@substr$length\n");
    printf("DEFVAR LF@substr$result\n");
    printf("DEFVAR LF@substr$index\n");
    printf("DEFVAR LF@substr$char\n");
    printf("DEFVAR LF@substr$invalid\n");

    printf("POPS LF@substr$value\n");
    printf("POPS LF@substr$start\n");
    printf("POPS LF@substr$end\n");

    printf("STRLEN LF@substr$length LF@substr$value\n");
    printf("MOVE LF@substr$invalid bool@false\n");

    // start < 0
    printf("LT LF@substr$invalid LF@substr$start int@0\n");

    // end < 0
    printf("LT LF@substr$char LF@substr$end int@0\n");
    printf("OR LF@substr$invalid LF@substr$invalid LF@substr$char\n");

    // start > end
    printf("GT LF@substr$char LF@substr$start LF@substr$end\n");
    printf("OR LF@substr$invalid LF@substr$invalid LF@substr$char\n");

    // start >= length
    printf("GT LF@substr$char LF@substr$start LF@substr$length\n");
    printf("EQ LF@substr$result LF@substr$start LF@substr$length\n");
    printf("OR LF@substr$char LF@substr$char LF@substr$result\n");
    printf("OR LF@substr$invalid LF@substr$invalid LF@substr$char\n");

    // end > length
    printf("GT LF@substr$char LF@substr$end LF@substr$length\n");
    printf("OR LF@substr$invalid LF@substr$invalid LF@substr$char\n");

    printf("JUMPIFEQ $substr$error LF@substr$invalid bool@true\n");

    printf("MOVE LF@substr$result string@\n");
    printf("MOVE LF@substr$index LF@substr$start\n");

    printf("LABEL $substr$loop\n");
    printf("JUMPIFEQ $substr$done LF@substr$index LF@substr$end\n");
    printf("GETCHAR LF@substr$char LF@substr$value LF@substr$index\n");
    printf("CONCAT LF@substr$result LF@substr$result LF@substr$char\n");
    printf("ADD LF@substr$index LF@substr$index int@1\n");
    printf("JUMP $substr$loop\n");

    printf("LABEL $substr$done\n");
    printf("PUSHS LF@substr$result\n");
    printf("RETURN\n");

    printf("LABEL $substr$error\n");
    printf("EXIT int@40\n");
}
