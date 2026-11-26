// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)

#include "gen_func.h"
#include "codegen_helper.h"
#include "scanner.h"
#include <string.h>

void create_head() {
  printf(".IFJcode26\n");
  printf("JUMP main\n\n");
}

void gen_params_declaration(char *var_name) {
  gen_defvar();
  printf("%s", var_name);
  gen_new_line();
}

void gen_new_line() { printf("\n"); }

void gen_defvar() { printf("DEFVAR "); }

void gen_createframe() { printf("CREATEFRAME\n"); }

void gen_popframe() { printf("POPFRAME\n"); }

void gen_return() { printf("RETURN\n"); }

void gen_pushframe() { printf("PUSHFRAME\n"); }

void gen_pops(char *var) {
  printf("POPS ");
  if (var != NULL) {
    printf("%s", var);
  }
  gen_new_line();
}

void gen_break() { printf("BREAK\n"); }

void gen_adds() { printf("ADDS\n"); }

void gen_subs() { printf("SUBS\n"); }

void gen_muls() { printf("MULS\n"); }

void gen_divs() { printf("DIVS\n"); }

void gen_idivs() { printf("IDIVS\n"); }

void gen_LTs() { printf("LTS\n"); }

void gen_GTs() { printf("GTS\n"); }

void gen_EQs() { printf("EQS\n"); }

void gen_NOTs() { printf("NOTS\n"); }

void gen_int2floats() { printf("INT2FLOATS\n"); }

void gen_tostr(ASTNode *arg) {
  switch (arg->inferred_type) {
  case TYPE_INT:
    printf("INT2STRS\n");
    break;
  case TYPE_FLOAT:
    printf("FLOAT2STRS\n");
    break;
  case TYPE_BOOL: {
    int id = fresh_id();
    printf("POPS %s\n", scratch());
    printf("JUMPIFEQ $bool$true$%d %s bool@true\n", id, scratch());
    printf("PUSHS string@false\n");
    printf("JUMP $bool$end$%d\n", id);
    printf("LABEL $bool$true$%d\n", id);
    printf("PUSHS string@true\n");
    printf("LABEL $bool$end$%d\n", id);
    break;
  }
  default:
    break;
  }
}

/* Reads a value into the scratch variable. EOF or bad input gives nil, which
 * becomes the default. */
void gen_read_builtin(char *type, char *default_value) {
  int id = fresh_id();
  printf("READ %s %s\n", scratch(), type);
  printf("JUMPIFNEQ $read$ok$%d %s nil@nil\n", id, scratch());
  printf("MOVE %s %s\n", scratch(), default_value);
  printf("LABEL $read$ok$%d\n", id);
  printf("PUSHS %s\n", scratch());
}

/*
Functions where something needs to be passed as an argument, like a label or a
variable name
*/
// label
void gen_jumpifeqs(char *label) { printf("JUMPIFEQS %s\n", label); }

void gen_pushs(ASTNode *node) {
  printf("PUSHS ");
  gen_argument(node);
  gen_new_line();
}

void gen_argument(ASTNode *arg_node) {
  Token *token = get_token_from_node(arg_node);
  if (token->type == TOKEN_IDENTIFIER) {
    printf("%s", gen_string_from_node(arg_node));
  } else {
    gen_data_type(arg_node);
  }
}

void gen_data_type(ASTNode *node) {
  Token *token = get_token_from_node(node);
  switch (token->type) {
  case TOKEN_INTEGER:
    printf("int@%d", token->attribute.integer_value);
    break;
  case TOKEN_DOUBLE:
    printf("float@%a", token->attribute.double_value);
    break;
  case TOKEN_STRING:
    printf("string@");
    gen_escaped_string(token->attribute.string_value);
    break;
  case TOKEN_TRUE:
    printf("bool@true");
    break;
  case TOKEN_FALSE:
    printf("bool@false");
    break;
  default:
    break;
  }
}

void gen_newarray(char *var, char *symbs) {
  printf("NEWARRAY %s %s\n", var, symbs);
}

void gen_arrayget(char *var, char *var_a, char *symb_index) {
  printf("ARRAYGET %s %s %s\n", var, var_a, symb_index);
}

void gen_arrayset(char *var_a, char *symb_index, char *symb_val) {
  printf("ARRAYSET %s %s %s\n", var_a, symb_index, symb_val);
}

void gen_label(char *label) { printf("LABEL %s\n", label); }

void gen_jump(char *label) { printf("JUMP %s\n", label); }

void gen_exit() { printf("EXIT "); }

void gen_call(ASTNode *node) {
  char *label = gen_string_from_node(node);

  if (matching_string(label, "readInt")) {
    gen_read_builtin("int", "int@0");
  } else if (matching_string(label, "readDouble")) {
    gen_read_builtin("float", "float@0x0p+0");
  } else if (matching_string(label, "readLine")) {
    gen_read_builtin("string", "string@");
  } else if (matching_string(label, "readBoolean")) {
    gen_read_builtin("bool", "bool@false");
  } else if (matching_string(label, "strLength")) {
    printf("POPS %s\n", scratch());
    printf("STRLEN %s %s\n", scratch(), scratch());
    printf("PUSHS %s\n", scratch());
  } else if (matching_string(label, "toStr")) {
    gen_tostr(node->right);
  } else if (matching_string(label, "toInt")) {
    printf("FLOAT2INTS\n");
  } else if (matching_string(label, "chr")) {
    printf("INT2CHARS\n");
  } else {
    printf("CALL %s\n", label);
  }
}

void gen_builtin_ord() {
  gen_label("ord");
  gen_createframe();
  gen_pushframe();
  printf("DEFVAR LF@ord$value\n");
  printf("DEFVAR LF@ord$index\n");
  printf("DEFVAR LF@ord$length\n");
  printf("DEFVAR LF@ord$char\n");
  printf("DEFVAR LF@ord$result\n");
  printf("DEFVAR LF@ord$tmp\n");
  printf("DEFVAR LF@ord$invalid\n");

  // arguments were pushed value first, index last => index is on top
  printf("POPS LF@ord$value\n");
  printf("POPS LF@ord$index\n");

  printf("STRLEN LF@ord$length LF@ord$value\n");
  printf("MOVE LF@ord$invalid bool@false\n");

  // invalid if index < 0
  printf("LT LF@ord$tmp LF@ord$index int@0\n");
  printf("OR LF@ord$invalid LF@ord$invalid LF@ord$tmp\n");

  // invalid if index > length  (also covers empty string)
  printf("GT LF@ord$tmp LF@ord$index LF@ord$length\n");
  printf("OR LF@ord$invalid LF@ord$invalid LF@ord$tmp\n");

  // invalid if index == length
  printf("EQ LF@ord$tmp LF@ord$index LF@ord$length\n");
  printf("OR LF@ord$invalid LF@ord$invalid LF@ord$tmp\n");

  printf("JUMPIFEQ $ord$invalid LF@ord$invalid bool@true\n");

  printf("GETCHAR LF@ord$char LF@ord$value LF@ord$index\n");
  printf("STRI2INT LF@ord$result LF@ord$char int@0\n");

  // push result while the local frame is still accessible
  printf("PUSHS LF@ord$result\n");
  gen_popframe();
  gen_return();

  printf("LABEL $ord$invalid\n");
  printf("PUSHS int@-1\n");
  gen_popframe();
  gen_return();
  gen_new_line();
}

void gen_builtin_substr() {
  gen_label("subStr");
  gen_createframe();
  gen_pushframe();
  printf("DEFVAR LF@substr$value\n");
  printf("DEFVAR LF@substr$start\n");
  printf("DEFVAR LF@substr$end\n");
  printf("DEFVAR LF@substr$length\n");
  printf("DEFVAR LF@substr$result\n");
  printf("DEFVAR LF@substr$index\n");
  printf("DEFVAR LF@substr$char\n");
  printf("DEFVAR LF@substr$tmp\n");
  printf("DEFVAR LF@substr$invalid\n");

  // last argument is on top of the stack: end, start, value
  printf("POPS LF@substr$value\n");
  printf("POPS LF@substr$start\n");
  printf("POPS LF@substr$end\n");

  printf("STRLEN LF@substr$length LF@substr$value\n");
  printf("MOVE LF@substr$invalid bool@false\n");

  // start < 0
  printf("LT LF@substr$tmp LF@substr$start int@0\n");
  printf("OR LF@substr$invalid LF@substr$invalid LF@substr$tmp\n");

  // end < 0
  printf("LT LF@substr$tmp LF@substr$end int@0\n");
  printf("OR LF@substr$invalid LF@substr$invalid LF@substr$tmp\n");

  // start > end
  printf("GT LF@substr$tmp LF@substr$start LF@substr$end\n");
  printf("OR LF@substr$invalid LF@substr$invalid LF@substr$tmp\n");

  // start >= length  (start > length OR start == length)
  printf("GT LF@substr$tmp LF@substr$start LF@substr$length\n");
  printf("OR LF@substr$invalid LF@substr$invalid LF@substr$tmp\n");
  printf("EQ LF@substr$tmp LF@substr$start LF@substr$length\n");
  printf("OR LF@substr$invalid LF@substr$invalid LF@substr$tmp\n");

  // end > length
  printf("GT LF@substr$tmp LF@substr$end LF@substr$length\n");
  printf("OR LF@substr$invalid LF@substr$invalid LF@substr$tmp\n");

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
  gen_popframe();
  gen_return();

  printf("LABEL $substr$error\n");
  printf("EXIT int@40\n");
  gen_new_line();
}
