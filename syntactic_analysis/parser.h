// Compiler of IFJ26 imperative language
// Author: Michal Kopček (xkopcem00)

#ifndef PARSER_H
#define PARSER_H
#include "../scanner/scanner.h"
#include "expression.h"
#include "ast.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

extern Token current_token;

Token get_token_buffered(void);
void unget_token(Token t);

bool match(TokenType expected_type); // check if the current token matches the expected type and advance to the next token if it does

int parse(); // return an error code or 0 if everithing runs smoothly

ASTNode* parse_func_call();

// function for non-terminals
ASTNode* parse_program(); 
bool parse_prolog(); 
bool parse_functions(ASTNode **out_functions); 
ASTNode* parse_function(); 
bool parse_main_annotation(bool *is_main); 
ASTNode* parse_params_opt(); 
ASTNode* parse_param(); 
bool parse_return_type_opt(DataType *out); 
bool parse_type(DataType *out); 
ASTNode* parse_function_body(); 
ASTNode* parse_statements(); 
ASTNode* parse_statement(); 
bool parse_type_opt(DataType *out); 
ASTNode* parse_expr_or_call();
ASTNode* parse_expr_or_call_opt(); 
ASTNode* parse_args_opt(); 
ASTNode* parse_term(); 
void parser_cleanup(void);

#endif