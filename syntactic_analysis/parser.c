// Compiler of IFJ26 imperative language
// Author: Michal Kopček (xkopcem00)

#include "parser.h"

Token current_token;
ASTNode *global_ast_root = NULL;

static Token unget_buffer;
static bool has_unget = false;

static bool uses_ord = false;
static bool uses_substr = false;

static void note_builtin_use(char *name) {
    if (name == NULL) return;
    if (matching_string(name, "ord") == 0) {
        uses_ord = true;
    }
    else if (matching_string(name, "subStr") == 0) {
        uses_substr = true;
    }    
}

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
    uses_ord = false;
    uses_substr = false;

    scanner_init();

    current_token = get_token_buffered();
    global_ast_root = parse_program();
    if (global_ast_root == NULL) {
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
        ASTNode *root = create_ast_node(AST_NODE_PROGRAM, functions_node, NULL, NULL, root_token);
        if (root != NULL) {
            root->uses_substr = uses_substr;

            root->uses_ord = uses_ord;
        }
        return root;
    }
    return NULL;
}

bool parse_prolog() {
    if (!match(TOKEN_IMPORT)) return false;
    
    if (current_token.type != TOKEN_IDENTIFIER) return false;
    if (matching_string(current_token.attribute.string_value, "ifj26") != 0) {
        free(current_token.attribute.string_value);
        return false;
    }
    free(current_token.attribute.string_value);
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

    DataType ret_type;
    if (!parse_return_type_opt(&ret_type)) return NULL;
    if (!match(TOKEN_ASSIGN)) return NULL;
    
    ASTNode *function_body_node = parse_function_body();
    if (function_body_node == NULL) return NULL;

    ASTNode *func_node = create_ast_node(AST_NODE_FUNCTION, params_node, function_body_node, NULL, function_token);
    if (func_node != NULL) {
        func_node->is_main = is_main;
        func_node->inferred_type = ret_type;
    }
    
    return func_node;
}

bool parse_main_annotation(bool *is_main) {
    *is_main = false;
    if (match(TOKEN_AT)) {
        if (current_token.type != TOKEN_IDENTIFIER) return false;
        if (matching_string(current_token.attribute.string_value, "main") != 0) {
            free(current_token.attribute.string_value);
            return false;
        }
        free(current_token.attribute.string_value);

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
        if (current_token.type == TOKEN_RPAREN && current_token.newline_before) break;
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
    if (current_token.type != TOKEN_IDENTIFIER) return NULL;
    Token id_token = current_token;
    current_token = get_token_buffered();
    
    if (!match(TOKEN_COLON)) return NULL;

    DataType param_type;
    if (!parse_type(&param_type)) return NULL;
    
    ASTNode *param_node = create_ast_node(AST_NODE_PARAM, NULL, NULL, NULL, id_token);
    param_node->inferred_type = param_type;
    return param_node;
}

bool parse_return_type_opt(DataType *out) {
    *out = TYPE_UNKNOWN;
    if (match(TOKEN_COLON)) {
        return parse_type(out);
    }
    return true;
}

bool parse_type(DataType *out) {
    DataType t;
    switch (current_token.type) {
        case TOKEN_TYPE_INT:     t = TYPE_INT;    break;
        case TOKEN_TYPE_DOUBLE:  t = TYPE_FLOAT;  break;
        case TOKEN_TYPE_STRING:  t = TYPE_STRING; break;
        case TOKEN_TYPE_UNIT:    t = TYPE_VOID;   break;
        case TOKEN_TYPE_BOOLEAN: t = TYPE_BOOL;   break;
        default: return false;
    }
    current_token = get_token_buffered();
    if (out != NULL) *out = t;
    return true;
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
        bool is_block_stmt = (current_token.type == TOKEN_IF || current_token.type == TOKEN_WHILE);

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
        if (!is_block_stmt) {
            if (!match(TOKEN_EOL) && current_token.type != TOKEN_DEDENT && current_token.type != TOKEN_EOF) {
                return NULL;
            }
        }
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
        return parse_if_statement();
    }
    else if (current_token.type == TOKEN_VAL || current_token.type == TOKEN_VAR) {
        bool is_const = (current_token.type == TOKEN_VAL);
        current_token = get_token_buffered();
        
        if (current_token.type != TOKEN_IDENTIFIER) return NULL;
        Token id_token = current_token;
        current_token = get_token_buffered();
        
        DataType declared_type;
        if (!parse_type_opt(&declared_type)) return NULL;
        if (!match(TOKEN_ASSIGN)) return NULL;
        
        ASTNode *expr_node = parse_expr_or_call();
        if (expr_node == NULL) return NULL;
        
        ASTNode *var_node = create_ast_node(AST_NODE_VAR, NULL, NULL, NULL, id_token);
        
        Token empty_id = id_token;
        empty_id.attribute.string_value = NULL;

        ASTNode *var_decl_node= create_ast_node(AST_NODE_VAR_DECL, var_node, expr_node, NULL, empty_id);
        
        if (var_decl_node != NULL) {
            var_decl_node->is_const = is_const;
            var_decl_node->inferred_type = declared_type;
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

            Token empty_id = id_token;
            empty_id.attribute.string_value = NULL;

            return create_ast_node(AST_NODE_ASSIGN, var_node, expr_node, NULL, empty_id);
        }
        else {
            unget_token(next_token);
            return parse_expr_or_call();
        }
    }
    else {
        return parse_expr_or_call();
    }
}

ASTNode *parse_if_statement() {
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
    if (then_node == NULL) {
        return NULL;
    }    
    if (!match(TOKEN_DEDENT)) return NULL;

    ASTNode *else_node = NULL;
    if (match(TOKEN_ELSE)) {
        if (current_token.type == TOKEN_IF) {
            else_node = parse_if_statement();
            if (else_node == NULL) return NULL;
        }
        else {
            if (!match(TOKEN_EOL)) return NULL;
            if (!match(TOKEN_INDENT)) return NULL;

            else_node = parse_statements();
            if (else_node == NULL) return NULL;
            if (!match(TOKEN_DEDENT)) return NULL;
        }
    }        
    return create_ast_node(AST_NODE_IF, cond_node, then_node, else_node, if_token);
}

bool parse_type_opt(DataType *out) {
    *out = TYPE_UNKNOWN;
    if (match(TOKEN_COLON)) {
        return parse_type(out);
    }
    return true;
}

ASTNode* parse_func_call() {
    if (current_token.type != TOKEN_IDENTIFIER) return NULL;
    Token id_token = current_token;
    note_builtin_use(id_token.attribute.string_value);
    current_token = get_token_buffered();

    if (!match(TOKEN_LPAREN)) return NULL;

    ASTNode *args_node = NULL;
    if (current_token.type != TOKEN_RPAREN) {
        args_node = parse_args_opt();
        if (args_node == NULL) {
            return NULL;
        }
    }

    if (!match(TOKEN_RPAREN)) return NULL;
    return create_ast_node(AST_NODE_CALL, NULL, args_node, NULL, id_token);
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

ASTNode* parse_args_opt() {
    if (current_token.type == TOKEN_RPAREN) return NULL;

    ASTNode *first_arg = parse_expression();
    if (first_arg == NULL) return NULL;

    ASTNode *last_arg = first_arg;
    while (match(TOKEN_COMMA)) {
        if (current_token.type == TOKEN_RPAREN && current_token.newline_before) break;
        ASTNode *next_arg = parse_expression();
        if (next_arg == NULL) return NULL;

        last_arg->next = next_arg;
        next_arg->prev = last_arg;
        last_arg = next_arg;
    }
    return first_arg;
}

// can be removed after adding FUNEXP
ASTNode* parse_term() {
    if (current_token.type == TOKEN_IDENTIFIER || current_token.type == TOKEN_INTEGER || 
        current_token.type == TOKEN_DOUBLE || current_token.type == TOKEN_STRING) {
        
        Token term_token = current_token;
        current_token = get_token_buffered();
        return create_ast_node(AST_NODE_TERM, NULL, NULL, NULL, term_token); 
    }
    else if (current_token.type == TOKEN_LPAREN) {
        Token lparen_token = current_token;
        current_token = get_token_buffered();
        if (!match(TOKEN_RPAREN)) {
            return NULL;
        }
        return create_ast_node(AST_NODE_UNIT, NULL, NULL, NULL, lparen_token);
    }
    return NULL;
}

void parser_cleanup(void) {
    ast_cleanup();
    global_ast_root = NULL;
}