#ifndef PARSER_H
#define PARSER_H
#include <stdbool.h>
#include "../scanner/scanner.h"

extern Token current_token;

Token get_token_buffered(void);
void unget_token(Token t);

bool match(TokenType expected_type); // check if the current token matches the expected type and advance to the next token if it does

int parse(); // return an error code or 0 if everithing runs smoothly

// function for non-terminals
ASTNode* parse_program(); 
bool parse_prolog(); 
ASTNode* parse_functions(); 
ASTNode* parse_function(); 
bool parse_main_annotation(bool *is_main); 
ASTNode* parse_params_opt(); 
ASTNode* parse_param(); 
bool parse_return_type_opt(); 
bool parse_type(); 
ASTNode* parse_function_body(); 
ASTNode* parse_statements(); 
ASTNode* parse_statement(); 
bool parse_type_opt(); 
ASTNode* parse_expr_or_call();
ASTNode* parse_expr_or_call_opt(); 
ASTNode* parse_args_opt(); 
ASTNode* parse_term(); 

#endif