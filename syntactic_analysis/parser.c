#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "expression.h"
#include "ast.h"

Token current_token;
ASTNode *global_ast_root = NULL;

int parse() {
    current_token = get_next_token();
    if (current_token.type == TOKEN_ERROR) {
        return 1; // Lexical error
    }
    global_ast_root = parse_program();
    if (global_ast_root == NULL) {
        if (current_token.type == TOKEN_ERROR) {
            return 1; // Lexical error
        }
        return 2; // Syntax error
    }
    return 0; // Successful parsing
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
    if (current_token.type == TOKEN_IMPORT) {
        current_token = get_next_token();
        if (current_token.type == TOKEN_IDENTIFIER || strcmp(current_token.attribute.string_value, "ifj26") == 0) {
            current_token = get_next_token();
            if (current_token.type == TOKEN_DOT) {
                current_token = get_next_token();
                if (current_token.type == TOKEN_MUL) {
                    current_token = get_next_token();
                    if (current_token.type == TOKEN_EOL) {
                        current_token = get_next_token();
                        return true;
                    }
                }
            }
        }
    }
    return false;
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
    if (parse_main_annotation() == false) {
        return NULL;    
    }
    if (current_token.type == TOKEN_DEF) {
        current_token = get_next_token();
        if (current_token.type == TOKEN_IDENTIFIER) {
            Token function_token = current_token;
            current_token = get_next_token();
            if (current_token.type == TOKEN_LPAREN) {
                current_token = get_next_token();
                if (parse_params_opt() == false) {
                    return NULL;
                }
                if (current_token.type == TOKEN_RPAREN) {
                    current_token = get_next_token();
                    if (parse_return_type_opt() == false) {
                        return NULL;
                    } 
                    if (current_token.type == TOKEN_ASSIGN) {
                        current_token = get_next_token();

                        ASTNode *function_body_node = parse_function_body();
                        if (function_body_node == NULL) {
                            return NULL;
                        }
                        return create_ast_node(AST_NODE_FUNCTION, function_body_node, NULL, NULL, function_token);
                    }
                }
            }
        }
    }
    return NULL;
}

bool parse_main_annotation() {
    if (current_token.type == TOKEN_AT) {
        current_token = get_next_token();
        if (current_token.type == TOKEN_IDENTIFIER && strcmp(current_token.attribute.string_value, "main") == 0) {
            current_token = get_next_token();
            return true;
        }
        return false;
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
    if (current_token.type == TOKEN_COMMA) {
        current_token = get_next_token();
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
    if (current_token.type == TOKEN_IDENTIFIER) {
        current_token = get_next_token();
        if (current_token.type == TOKEN_COLON) {
            current_token = get_next_token();
            if (parse_type() == false) {
                return false;
            }
            return true;
        }
    }
    return false;
}

bool parse_return_type_opt() {
    if (current_token.type == TOKEN_COLON) {
        current_token = get_next_token();
        if (parse_type() == false) {
            return false;
        }
    }
    return true;
}

bool parse_type() {
    if (current_token.type == TOKEN_TYPE_INT || current_token.type == TOKEN_TYPE_DOUBLE || current_token.type == TOKEN_TYPE_STRING || current_token.type == TOKEN_TYPE_UNIT) {
        current_token = get_next_token();
        return true;
    }
    return false;
}

ASTNode* parse_function_body() {
    if (current_token.type == TOKEN_EOL) {
        current_token = get_next_token();
        if (current_token.type == TOKEN_INDENT) {
            current_token = get_next_token();

            ASTNode *statements_node = parse_statements();
            if (statements_node == NULL) {
                return NULL;
            }
            if (current_token.type == TOKEN_DEDENT) {
                current_token = get_next_token();
                return statements_node;
            }
        }
        return NULL;
    }
    else {
        ASTNode *stmt_node = parse_statement();
        if (stmt_node == NULL) {
        return NULL;
        }
        if (current_token.type == TOKEN_EOL) {
            current_token = get_next_token();
            return stmt_node;
        }
        return NULL;
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
        if (current_token.type == TOKEN_EOL) {
            current_token = get_next_token();
        }
        else {
            return NULL;
        }
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
        if (current_token.type == TOKEN_DO) {
            current_token = get_next_token();
            if (current_token.type == TOKEN_EOL) {
                current_token = get_next_token();
                if (current_token.type == TOKEN_INDENT) {
                    current_token = get_next_token();

                    ASTNode *body_node = parse_statements();
                    if (body_node == NULL) {
                        return NULL;
                    }
                    if (current_token.type == TOKEN_DEDENT) {
                        current_token = get_next_token();
                        return create_ast_node(AST_NODE_WHILE, cond_node, body_node, NULL, while_token);
                    }
                }
            }
        }
        return NULL;
    }
    else if (current_token.type == TOKEN_IF) {
        Token if_token = current_token;
        current_token = get_next_token();

        ASTNode *cond_node = parse_expression();
        if (cond_node == NULL) {
            return NULL;
        }
        if (current_token.type == TOKEN_THEN) {
            current_token = get_next_token();
            if (current_token.type == TOKEN_EOL) {
                current_token = get_next_token();
                if (current_token.type == TOKEN_INDENT) {
                    current_token = get_next_token();

                    ASTNode *then_node = parse_statements();
                    if (then_node == NULL) {
                        return NULL;
                    }
                    if (current_token.type == TOKEN_DEDENT) {
                        current_token = get_next_token();
                        if (current_token.type == TOKEN_ELSE) {
                            current_token = get_next_token();
                            if (current_token.type == TOKEN_EOL) {
                                current_token = get_next_token();
                                if (current_token.type == TOKEN_INDENT) {
                                    current_token = get_next_token();

                                    ASTNode *else_node = parse_statements();
                                    if (else_node == NULL) {
                                        return NULL;
                                    }
                                    if (current_token.type == TOKEN_DEDENT) {
                                        current_token = get_next_token();
                                        return create_ast_node(AST_NODE_IF, cond_node, then_node, else_node, if_token);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return NULL;
    }
    else if (current_token.type == TOKEN_VAL || current_token.type == TOKEN_VAR) {
        current_token = get_next_token();
        if (current_token.type == TOKEN_IDENTIFIER) {
            Token id_token = current_token;
            current_token = get_next_token();
            if (parse_type_opt() == false) {
                return NULL;
            }
            if (current_token.type == TOKEN_ASSIGN) {
                current_token = get_next_token();

                ASTNode *expr_node = parse_expr_or_call();
                if (expr_node == NULL) {
                    return NULL;
                }
                return create_ast_node(AST_NODE_VAR_DECL, NULL, expr_node, NULL, id_token);
            }
            return NULL;
        }
        return NULL;
    }
    else {
        return parse_expr_or_call();
    }
}

bool parse_type_opt() {
    if (current_token.type == TOKEN_COLON) {
        current_token = get_next_token();
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
    if (current_token.type == TOKEN_COMMA) {
        current_token = get_next_token();
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
    if (current_token.type == TOKEN_IDENTIFIER || current_token.type == TOKEN_INTEGER || current_token.type == TOKEN_DOUBLE || current_token.type == TOKEN_STRING) {
        current_token = get_next_token();
        return true;
    }
    else if (current_token.type == TOKEN_LPAREN) {
        current_token = get_next_token();
        if (current_token.type == TOKEN_RPAREN) {
            current_token = get_next_token();
            return true;
        }
        return false;
    }
    return false;
}