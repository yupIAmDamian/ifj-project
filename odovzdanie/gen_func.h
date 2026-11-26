// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)

#ifndef GEN_FUNC_H
#define GEN_FUNC_H

#include "ast.h"
#include <stddef.h>
#include <stdio.h>

/**
 * @brief Global variable storing the name of the current function.
 */
extern char *current_func_name;

/**
 * @brief Generates the variable declaration instruction for a function
 * parameter.
 * @param var_name Full frame-prefixed name of the parameter variable.
 */
void gen_params_declaration(char *var_name);

/**
- @brief Generates the mandatory IFJcode26 header and initial jump to the main
function.
 */
void create_head();

/**
- @brief Prints a newline character to standard output.
 */
void gen_new_line();

/**
- @brief Prints the DEFVAR instruction prefix without a trailing space or
operand.
 */
void gen_defvar();

/**
- @brief Generates the CREATEFRAME instruction to allocate a new temporary
frame.
 */
void gen_createframe();

/**
- @brief Generates the POPFRAME instruction to pop the top frame from the frame
stack into TF.
 */
void gen_popframe();

/**
- @brief Generates the RETURN instruction to return execution flow from a
function call.
 */
void gen_return();

/**
- @brief Generates the PUSHFRAME instruction to push the temporary frame onto
the frame stack as LF.
 */
void gen_pushframe();

/**
- @brief Generates the POPS instruction to pop the top value from the data stack
into a variable.
- @param var Target variable identifier, or NULL to emit only the opcode.
 */
void gen_pops(char *var);

/**
- @brief Generates the BREAK instruction for interpreter debugging.
 */
void gen_break();

/**
- @brief Generates the ADDS stack instruction to add the top two values on the
data stack.
 */
void gen_adds();

/**
- @brief Generates the SUBS stack instruction to subtract the top two values on
the data stack.
 */
void gen_subs();

/**
- @brief Generates the MULS stack instruction to multiply the top two values on
the data stack.
 */
void gen_muls();

/**
- @brief Generates the DIVS stack instruction for floating-point division on the
data stack.
 */
void gen_divs();

/**
- @brief Generates the IDIVS stack instruction for integer division on the data
stack.
 */
void gen_idivs();

/**
- @brief Generates the LTS stack instruction for less-than comparison on the
data stack.
 */
void gen_LTs();

/**
- @brief Generates the GTS stack instruction for greater-than comparison on the
data stack.
 */
void gen_GTs();

/**
- @brief Generates the EQS stack instruction for equality comparison on the data
stack.
 */
void gen_EQs();

/**
- @brief Generates the NOTS stack instruction for negation on the data stack.
 */
void gen_NOTs();

/**
- @brief Generates the INT2FLOATS stack instruction to convert an integer on top
of the stack to float.
 */
void gen_int2floats();

/**
- @brief Generates instructions to convert an integer, float, or boolean value
on the stack to a string.
- @param arg Pointer to the AST node of the argument being converted.
 */
void gen_tostr(ASTNode *arg);

/**
- @brief Generates instructions for built-in input functions with fallback to a
default value on nil/EOF.
- @param type Target IFJcode26 input type string ("int", "float", "string", or
"bool").
- @param default_value Formatted IFJcode26 literal used if reading fails or hits
EOF.
 */
void gen_read_builtin(char *type, char *default_value);

/**
- @brief Generates the JUMPIFEQS instruction to conditionally jump if the top
two stack values are equal.
- @param label Target label name to jump to.
 */
void gen_jumpifeqs(char *label);

/**
- @brief Generates the PUSHS instruction to push a variable or literal onto the
data stack.
- @param node Pointer to the AST node representing the operand to push.
 */
void gen_pushs(ASTNode *node);

/**
- @brief Prints a formatted IFJcode26 operand from an AST node.
- @param arg_node Pointer to the AST node representing the argument.
 */
void gen_argument(ASTNode *arg_node);

/**
- @brief Prints a typed IFJcode26 constant literal (int, float, string, or bool)
from an AST node.
- @param node Pointer to the AST node containing the literal token.
 */
void gen_data_type(ASTNode *node);

/**
- @brief Generates a LABEL instruction defining a jump target.
- @param label Name of the label to define.
 */
void gen_label(char *label);

/**
- @brief Generates an JUMP instruction to a specified label.
- @param label Target label name.
 */
void gen_jump(char *label);

/**
- @brief Prints the EXIT instruction prefix without an exit code operand.
 */
void gen_exit();

/**
- @brief Generates instructions for calling either a built-in function or a user
defined function.
- @param node Pointer to the AST_NODE_CALL node.
 */
void gen_call(ASTNode *node);

/**
- @brief Generates implementation of the built-in ord(s, i) function.
 */
void gen_builtin_ord();

/**
- @brief Generates implementation of the built-in subStr(s, i, n) function.
 */
void gen_builtin_substr();

#endif