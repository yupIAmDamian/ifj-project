// Compiler of IFJ26 imperative language
// Author: Michal Kopček (xkopcem00)
#include "parser.h"

Token current_token;
ASTNode *global_ast_root = NULL;

static Token unget_buffer;
static bool has_unget = false;

void unget_token(Token t) {
    unget_buffer = t;
    has_unget = true;
}

Token get_token_buffered() {
    if (has_unget) {
        has_unget = false;
        return unget_buffer;
    }
    return get_next_token();
}

bool match(TokenType expected_type) {
    if (current_token.type == expected_type) {
        current_token = get_token_buffered();
        return true;
    }
    return false;
}

int parse() {
    scanner_init();

    current_token = get_token_buffered();
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

    ASTNode *functions_node = NULL;
    if (!parse_functions(&functions_node)) {
        return NULL;
    }
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
    current_token = get_token_buffered();
    
    if (!match(TOKEN_DOT)) return false;
    if (!match(TOKEN_MUL)) return false;
    if (!match(TOKEN_EOL)) return false;
    
    return true;
}

bool parse_functions(ASTNode **out_functions) {
    ASTNode *first_function = NULL;
    ASTNode *last_function = NULL;
    *out_functions = NULL;

    while (current_token.type == TOKEN_AT || current_token.type == TOKEN_DEF) {

        ASTNode *function_node = parse_function();
        if (function_node == NULL) {
            return false; // syntax error
        }
        if (first_function == NULL) {
            first_function = function_node;
            last_function = function_node;
        } else {
            last_function->next = function_node;
            function_node->prev = last_function;
            last_function = function_node;
        }
    }
    *out_functions = first_function;
    return true;
}

ASTNode* parse_function() {
    bool is_main = false;
    if (!parse_main_annotation(&is_main)) return NULL;
    if (!match(TOKEN_DEF)) return NULL;
    
    if (current_token.type != TOKEN_IDENTIFIER) return NULL;
    Token function_token = current_token;
    current_token = get_token_buffered();
    
    if (!match(TOKEN_LPAREN)) return NULL;
    ASTNode *params_node = parse_params_opt();
    if (!match(TOKEN_RPAREN)) return NULL;
    if (!parse_return_type_opt()) return NULL;
    if (!match(TOKEN_ASSIGN)) return NULL;
    
    ASTNode *function_body_node = parse_function_body();
    if (function_body_node == NULL) return NULL;

    ASTNode *func_node = create_ast_node(AST_NODE_FUNCTION, params_node, function_body_node, NULL, function_token);
    if (func_node != NULL) {
        func_node->is_main = is_main;
    }
    
    return func_node;
}

bool parse_main_annotation(bool *is_main) {
    *is_main = false;
    if (match(TOKEN_AT)) {
        if (current_token.type != TOKEN_IDENTIFIER) return false;
        if (strcmp(current_token.attribute.string_value, "main") != 0) return false;

        *is_main = true;
        current_token = get_token_buffered(); 

        match(TOKEN_EOL);
    }
    return true;
}

ASTNode* parse_params_opt() {
    if (current_token.type == TOKEN_RPAREN) {
        return NULL;
    }
    
    ASTNode *first_param = parse_param();
    if (first_param == NULL) {
        return NULL; // syntax error
    }

    ASTNode *last_param = first_param;

    while (match(TOKEN_COMMA)) {
        ASTNode *next_param = parse_param();
        if (next_param == NULL) {
            return NULL; // syntax error
        }
        last_param->next = next_param;
        next_param->prev = last_param;
        last_param = next_param;
    }
    return first_param;
}

ASTNode* parse_param() {
    if (current_token.type != TOKEN_IDENTIFIER) return false;
    Token id_token = current_token;
    current_token = get_token_buffered();
    
    if (!match(TOKEN_COLON)) return NULL;
    if (parse_type() == false) { 
        return NULL; // syntax error
    }
    return create_ast_node(AST_NODE_PARAM, NULL, NULL, NULL, id_token);
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
        if (!match(TOKEN_EOL) && current_token.type != TOKEN_EOF) return NULL;
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
            stmt_node->prev = last_statement_node;
            last_statement_node = stmt_node;

            while (last_statement_node->next != NULL) {
                last_statement_node = last_statement_node->next;
            }
        }
        if (!match(TOKEN_EOL) && current_token.type != TOKEN_DEDENT && current_token.type != TOKEN_EOF) return NULL;
    }
    return first_statement_node;
}

ASTNode* parse_statement() {
    if (current_token.type == TOKEN_RETURN) {
        Token return_token = current_token;
        current_token = get_token_buffered();

        ASTNode *expr_node = parse_expr_or_call_opt();
        return create_ast_node(AST_NODE_RETURN, expr_node, NULL, NULL, return_token);
    }
    else if (current_token.type == TOKEN_WHILE) {
        Token while_token = current_token;
        current_token = get_token_buffered();

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
        current_token = get_token_buffered();

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
        bool is_const = (current_token.type == TOKEN_VAL);
        current_token = get_token_buffered();
        
        if (current_token.type != TOKEN_IDENTIFIER) return NULL;
        Token id_token = current_token;
        current_token = get_token_buffered();
        
        if (!parse_type_opt()) return NULL;
        if (!match(TOKEN_ASSIGN)) return NULL;
        
        ASTNode *expr_node = parse_expr_or_call();
        if (expr_node == NULL) return NULL;
        
        ASTNode *var_node = create_ast_node(AST_NODE_VAR, NULL, NULL, NULL, id_token);
        ASTNode *var_decl_node= create_ast_node(AST_NODE_VAR_DECL, var_node, expr_node, NULL, id_token);
        
        if (var_decl_node != NULL) {
            var_decl_node->is_const = is_const;
        }
        if (var_node != NULL) {
            var_node->is_const = is_const;
        }
        return var_decl_node;
    }
    else if (current_token.type == TOKEN_IDENTIFIER) {
        Token next_token = get_token_buffered();

        if (next_token.type == TOKEN_ASSIGN) {
            Token id_token = current_token;
            current_token = get_token_buffered();

            ASTNode *expr_node = parse_expr_or_call();
            if (expr_node == NULL) return NULL;

            ASTNode *var_node = create_ast_node(AST_NODE_VAR, NULL, NULL, NULL, id_token);
            return create_ast_node(AST_NODE_ASSIGN, var_node, expr_node, NULL, id_token);
        }
        else {
            unget_token(next_token);
            return parse_expr_or_call();
        }
    }
    else if (current_token.type == TOKEN_LPAREN) {
        Token lparen_token = current_token;
        current_token = get_token_buffered();
        if (!match(TOKEN_RPAREN)) {
            return NULL; // syntax error
        }
        return create_ast_node(AST_NODE_UNIT, NULL, NULL, NULL, lparen_token);
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
    if (current_token.type == TOKEN_IDENTIFIER) {
        Token next_token = get_token_buffered();
        if (next_token.type == TOKEN_LPAREN) {
            Token id_token = current_token;
            current_token = get_token_buffered();

            ASTNode *args_node = parse_args_opt();
            if (!match(TOKEN_RPAREN)) return NULL;
            return create_ast_node(AST_NODE_CALL, NULL, args_node, NULL, id_token);
        }
        else {
            unget_token(next_token);
            return parse_expression();
        }
    }
    return parse_expression();
}

ASTNode* parse_expr_or_call_opt() {
    if (current_token.type == TOKEN_EOL || current_token.type == TOKEN_DEDENT || current_token.type == TOKEN_EOF) {
        return NULL;
    }
    return parse_expr_or_call();
}

ASTNode* parse_args_opt() {
    if (current_token.type == TOKEN_RPAREN) return NULL; 

    ASTNode *first_arg = parse_term();
    if (first_arg == NULL) return NULL;

    ASTNode *last_arg = first_arg;
    while (match(TOKEN_COMMA)) {
        ASTNode *next_arg = parse_term();
        if (next_arg == NULL) return NULL;
        
        last_arg->next = next_arg;
        next_arg->prev = last_arg;
        last_arg = next_arg;
    }
    return first_arg;
}

ASTNode* parse_term() {
    if (current_token.type == TOKEN_IDENTIFIER || current_token.type == TOKEN_INTEGER || 
        current_token.type == TOKEN_DOUBLE || current_token.type == TOKEN_STRING) {
        
        Token term_token = current_token;
        current_token = get_token_buffered();
        return create_ast_node(AST_NODE_TERM, NULL, NULL, NULL, term_token); 
    }
    return NULL;
}