#ifndef PARSER_H
#define PARSER_H
#include <stdbool.h>
#include "scanner.h" // Definicia tokenu

extern Token current_token; // Aktualny token

// Hlavna funkcia analyzatora
int parse(); // Spusti cely proces syntaktickej analyzy a vrati chybovy kod alebo 0 ak bude vsetko ficat

// Funkcie pre neterminaly
// 'true' ak sa podarilo uspesne aplikovat pravidlo alebo epsilon
// 'false' ak narazi na syntakticku chybu
bool parse_program(); // <program> -> <prolog> <functions> EOF
bool parse_prolog(); // <prolog> -> import ifj26 . * EOL
bool parse_functions(); // <functions> -> <function> <functions> | epsilon
bool parse_function(); // <function> -> <main_annotation> def id (<params_opt>) <return_type_opt> = <function_body>
bool parse_main_annotation(); // <main_annotation> -> @main | epsilon
bool parse_params_opt(); // <params_opt> -> <param> <param_list> | epsilon
bool parse_param_list(); // <param_list> -> , <param> <param_list> | epsilon
bool parse_param(); // <param> -> id : <type>
bool parse_return_type_opt(); // <return_type_opt> -> : <type> | epsilon
bool parse_type(); // <type> -> Int | Double | String | Unit
bool parse_function_body(); // <function_body> -> <statement> EOL | EOL INDENT <statements> DEDENT
bool parse_statements(); // <statements> -> <statement> EOL <statements> | epsilon
bool parse_statement(); 
// <statement> -> val id <type_opt> = <expr_or_call> 
//              | var id <type_opt> = <expr_or_call> 
//              | id = <expr_or_call>
//              | id ( <args_opt> )
//              | return <expr_or_call_opt>
//              | if <expr> then EOL INDENT <statements> DEDENT else EOL INDENT <statements> DEDENT
//              | while <expr> do EOL INDENT <statements> DEDENT
//              | <expr>
bool parse_type_opt(); // <type_opt> -> : <type> | epsilon
bool parse_expr_or_call(); // <expr_or_call> -> id (<args_opt>) | <expr>
bool parse_expr_or_call_opt(); // <expr_or_call_opt> -> <expr_or_call> | epsilon
bool parse_args_opt(); // <args_opt> -> <term> <args_list> | epsilon
bool parse_args_list(); // <args_list> -> , <term> <args_list> | epsilon
bool parse_term(); // <term> -> id | Int | Double | String | ()

#endif