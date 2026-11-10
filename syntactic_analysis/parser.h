#ifndef PARSER_H
#define PARSER_H
#include <stdbool.h>
#include "scanner.h" 

extern Token current_token;

bool match(TokenType expected_type); // check if the current token matches the expected type and advance to the next token if it does

int parse(); // return an error code or 0 if everithing runs smoothly

// function for non-terminals
ASTNode* parse_program(); // <program> -> <prolog> <functions> EOF
bool parse_prolog(); // <prolog> -> import ifj26 . * EOL
ASTNode* parse_functions(); // <functions> -> <function> <functions> | epsilon
ASTNode* parse_function(); // <function> -> <main_annotation> def id (<params_opt>) <return_type_opt> = <function_body>
bool parse_main_annotation(); // <main_annotation> -> @main | epsilon
bool parse_params_opt(); // <params_opt> -> <param> <param_list> | epsilon
bool parse_param_list(); // <param_list> -> , <param> <param_list> | epsilon
bool parse_param(); // <param> -> id : <type>
bool parse_return_type_opt(); // <return_type_opt> -> : <type> | epsilon
bool parse_type(); // <type> -> Int | Double | String | Unit
ASTNode* parse_function_body(); // <function_body> -> <statement> EOL | EOL INDENT <statements> DEDENT
ASTNode* parse_statements(); // <statements> -> <statement> EOL <statements> | epsilon
ASTNode* parse_statement(); 
// <statement> -> val id <type_opt> = <expr_or_call> 
//              | var id <type_opt> = <expr_or_call> 
//              | id = <expr_or_call>
//              | id ( <args_opt> )
//              | return <expr_or_call_opt>
//              | if <expr> then EOL INDENT <statements> DEDENT else EOL INDENT <statements> DEDENT
//              | while <expr> do EOL INDENT <statements> DEDENT
//              | <expr>
bool parse_type_opt(); // <type_opt> -> : <type> | epsilon
ASTNode* parse_expr_or_call(); // <expr_or_call> -> id (<args_opt>) | <expr>
ASTNode* parse_expr_or_call_opt(); // <expr_or_call_opt> -> <expr_or_call> | epsilon
bool parse_args_opt(); // <args_opt> -> <term> <args_list> | epsilon
bool parse_args_list(); // <args_list> -> , <term> <args_list> | epsilon
bool parse_term(); // <term> -> id | Int | Double | String | ()

#endif