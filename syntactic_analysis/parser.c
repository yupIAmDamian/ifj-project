// Compiler of IFJ26 imperative language
// Author: Michal Kopček (xkopcem00)

#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "expression.h"
#include "ast.h"

Token current_token;
ASTNode *global_ast_root = NULL;

bool match(TokenType expected_type) {
    if (current_token.type == expected_type) {
        current_token = get_next_token();
        return true;
    }
    return false;
}

int parse() {
    scanner_init();

    current_token = get_next_token();
    if (current_token.type == TOKEN_ERROR) {
        return 1; // lexical error
    }
    global_ast_root = parse_program();
    if (global_ast_root == NULL) {
        if (current_token.type == TOKEN_ERROR) {
            return 1; // lexical error
        }
        return 2; // syntax error
    }
    return 0; // successful parsing
}

ASTNode* parse_program() { 
    if (!parse_prolog()) {
        return NULL;
    }

    ASTNode *functions_node = parse_functions();
    if (current_token.type == TOKEN_EOF) {
        Token root_token = current_token;
        return create_ast_node(AST_NODE_PROGRAM, functions_node, NULL, NULL, root_token);
    }
    return NULL;
}

bool parse_prolog() {
    if (!match(TOKEN_IMPORT)) return false;
    
    if (current_token.type != TOKEN_IDENTIFIER) return false;
    if (strcmp(current_token.attribute.string_value, "ifj26") != 0) return false;
    current_token = get_next_token();
    
    if (!match(TOKEN_DOT)) return false;
    if (!match(TOKEN_MUL)) return false;
    if (!match(TOKEN_EOL)) return false;
    
    return true;
}

ASTNode* parse_functions() {
    ASTNode *first_function = NULL;
    ASTNode *last_function = NULL;
    while (current_token.type == TOKEN_AT || current_token.type == TOKEN_DEF) {

        ASTNode *function_node = parse_function();
        if (function_node == NULL) {
            return NULL;
        }
        if (first_function == NULL) {
            first_function = function_node;
            last_function = function_node;
        } else {
            last_function->next = function_node;
            last_function = function_node;
        }
    }
    return first_function;
}

ASTNode* parse_function() {
    if (!parse_main_annotation()) return NULL;
    if (!match(TOKEN_DEF)) return NULL;
    
    if (current_token.type != TOKEN_IDENTIFIER) return NULL;
    Token function_token = current_token;
    current_token = get_next_token();
    
    if (!match(TOKEN_LPAREN)) return NULL;
    if (!parse_params_opt()) return NULL;
    if (!match(TOKEN_RPAREN)) return NULL;
    if (!parse_return_type_opt()) return NULL;
    if (!match(TOKEN_ASSIGN)) return NULL;
    
    ASTNode *function_body_node = parse_function_body();
    if (function_body_node == NULL) return NULL;
    
    return create_ast_node(AST_NODE_FUNCTION, function_body_node, NULL, NULL, function_token);
}

bool parse_main_annotation() {
    if (match(TOKEN_AT)) {
        if (current_token.type != TOKEN_IDENTIFIER) return false;
        if (strcmp(current_token.attribute.string_value, "main") != 0) return false;
        current_token = get_next_token(); 
    }
    return true;
}

bool parse_params_opt() {
    if (current_token.type == TOKEN_IDENTIFIER) {
        if (parse_param() == false) {
            return false;
        }
        if (parse_param_list() == false) {
            return false;
        }
    }
    return true;
}

bool parse_param_list() {
    if (match(TOKEN_COMMA)) {
        if (parse_param() == false) {
            return false;
        }
        if (parse_param_list() == false) {
            return false;
        }
    }
    return true;
}

bool parse_param() {
    if (current_token.type != TOKEN_IDENTIFIER) return false;
    current_token = get_next_token();
    
    if (!match(TOKEN_COLON)) return false;
    if (parse_type() == false) { 
        return false;
    }
    return true;
}

bool parse_return_type_opt() {
    if (match(TOKEN_COLON)) {
        if (parse_type() == false) {
            return false;
        }
    }
    return true;
}

bool parse_type() {
    if (match(TOKEN_TYPE_INT) || match(TOKEN_TYPE_DOUBLE) || match(TOKEN_TYPE_STRING) || match(TOKEN_TYPE_UNIT)) {
        return true;
    }
    return false;
}

ASTNode* parse_function_body() {
    if (match(TOKEN_EOL)) {
        if (!match(TOKEN_INDENT)) return NULL;
        
        ASTNode *statements_node = parse_statements();
        if (statements_node == NULL) return NULL;
        
        if (!match(TOKEN_DEDENT)) return NULL;
        return statements_node;
    } else {
        ASTNode *stmt_node = parse_statement();
        if (stmt_node == NULL) return NULL;
        if (!match(TOKEN_EOL)) return NULL;
        return stmt_node;
    }
}

ASTNode* parse_statements() {
    ASTNode *first_statement_node = NULL;
    ASTNode *last_statement_node = NULL;

    while (current_token.type != TOKEN_DEDENT && current_token.type != TOKEN_EOF) {
        ASTNode *stmt_node = parse_statement();
        if (stmt_node == NULL) {
            return NULL;
        }
        if (first_statement_node == NULL) {
            first_statement_node = stmt_node;
            last_statement_node = stmt_node;
        } else {
            last_statement_node->next = stmt_node;
            last_statement_node = stmt_node;

            while (last_statement_node->next != NULL) {
                last_statement_node = last_statement_node->next;
            }
        }
        if (!match(TOKEN_EOL)) return NULL;
    }
    return first_statement_node;
}

ASTNode* parse_statement() {
    if (current_token.type == TOKEN_RETURN) {
        Token return_token = current_token;
        current_token = get_next_token();

        ASTNode *expr_node = parse_expr_or_call_opt();
        return create_ast_node(AST_NODE_RETURN, expr_node, NULL, NULL, return_token);
    }
    else if (current_token.type == TOKEN_WHILE) {
        Token while_token = current_token;
        current_token = get_next_token();

        ASTNode *cond_node = parse_expression();
        if (cond_node == NULL) {
            return NULL;
        }
        if (!match(TOKEN_DO)) return NULL;
        if (!match(TOKEN_EOL)) return NULL;
        if (!match(TOKEN_INDENT)) return NULL;
        
        ASTNode *body_node = parse_statements();
        if (body_node == NULL) return NULL;
        
        if (!match(TOKEN_DEDENT)) return NULL;
        
        return create_ast_node(AST_NODE_WHILE, cond_node, body_node, NULL, while_token);
    }
    else if (current_token.type == TOKEN_IF) {
        Token if_token = current_token;
        current_token = get_next_token();

        ASTNode *cond_node = parse_expression();
        if (cond_node == NULL) {
            return NULL;
        }
        if (!match(TOKEN_THEN)) return NULL;
        if (!match(TOKEN_EOL)) return NULL;
        if (!match(TOKEN_INDENT)) return NULL;
        
        ASTNode *then_node = parse_statements();
        if (then_node == NULL) return NULL;
        
        if (!match(TOKEN_DEDENT)) return NULL;
        if (!match(TOKEN_ELSE)) return NULL;
        if (!match(TOKEN_EOL)) return NULL;
        if (!match(TOKEN_INDENT)) return NULL;
        
        ASTNode *else_node = parse_statements();
        if (else_node == NULL) return NULL;
        
        if (!match(TOKEN_DEDENT)) return NULL;
        
        return create_ast_node(AST_NODE_IF, cond_node, then_node, else_node, if_token);
    }
    else if (current_token.type == TOKEN_VAL || current_token.type == TOKEN_VAR) {
        current_token = get_next_token();
        
        if (current_token.type != TOKEN_IDENTIFIER) return NULL;
        Token id_token = current_token;
        current_token = get_next_token();
        
        if (!parse_type_opt()) return NULL;
        if (!match(TOKEN_ASSIGN)) return NULL;
        
        ASTNode *expr_node = parse_expr_or_call();
        if (expr_node == NULL) return NULL;
        
        return create_ast_node(AST_NODE_VAR_DECL, NULL, expr_node, NULL, id_token);
    }
    else {
        return parse_expr_or_call();
    }
}

bool parse_type_opt() {
    if (match(TOKEN_COLON)) {
        if (parse_type() == false) {
            return false;
        }
        return true;
    }
    return true;
}

ASTNode* parse_expr_or_call() {
    return parse_expression();
}

ASTNode* parse_expr_or_call_opt() {
    if (current_token.type == TOKEN_EOL || current_token.type == TOKEN_DEDENT || current_token.type == TOKEN_EOF) {
        return NULL;
    }
    return parse_expr_or_call();
}

bool parse_args_opt() {
    if (current_token.type == TOKEN_IDENTIFIER || current_token.type == TOKEN_INTEGER || current_token.type == TOKEN_DOUBLE || current_token.type == TOKEN_STRING || current_token.type == TOKEN_LPAREN) {
        if (parse_term() == false) {
            return false;
        }
        if (parse_args_list() == false) {
            return false;
        }
    }
    return true;
}

bool parse_args_list() {
    if (match(TOKEN_COMMA)) {
        if (parse_term() == false) {
            return false;
        }
        if (parse_args_list() == false) {
            return false;
        }
    }
    return true;
}

bool parse_term() {
    if (match(TOKEN_IDENTIFIER) || match(TOKEN_INTEGER) || match(TOKEN_DOUBLE) || match(TOKEN_STRING)) {
        return true;
    }
    else if (match(TOKEN_LPAREN)) {
        if (match(TOKEN_RPAREN)) {
            return true;
        }
        return false;
    }
    return false;
}