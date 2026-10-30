#include "parser.h"
#include <stdio.h>

Token current_token;

int parse() {
    current_token = get_next_token();
    if (current_token.type == TOKEN_ERROR) {
        return 1; // Lexical error
    }
    if (parse_program() == false) {
        if (current_token.type == TOKEN_ERROR) {
            return 1; // Lexical error
        }
        return 2; // Syntax error
    }
    return 0; // Successful parsing
}

bool parse_program() { 
    if (parse_prolog() && parse_functions() && current_token.type == TOKEN_EOF) {
        return true;
    }
        else {
            return false;
        }
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

bool parse_functions() {
    while (current_token.type == TOKEN_AT || current_token.type == TOKEN_DEF) {
        if (parse_function() == false) {
            return false;
        }
    }
    return true;
}

bool parse_function() {
    if (parse_main_annotation() == false) {
        return false;    
    }
    if (current_token.type == TOKEN_DEF) {
        current_token = get_next_token();
        if (current_token.type == TOKEN_IDENTIFIER) {
            current_token = get_next_token();
            if (current_token.type == TOKEN_LPAREN) {
                current_token = get_next_token();
                if (parse_params_opt() == false) {
                    return false;
                }
                if (current_token.type == TOKEN_RPAREN) {
                    current_token = get_next_token();
                    if (parse_return_type_opt() == false) {
                        return false;
                    } 
                    if (current_token.type == TOKEN_ASSIGN) {
                        current_token = get_next_token();
                        if (parse_function_body() == false) {
                            return false;
                        }
                        return true;
                    }
                }
            }
        }
    }
    return false;
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

bool parse_function_body() {
    if (current_token.type == TOKEN_EOL) {
        current_token = get_next_token();
        if (current_token.type == TOKEN_INDENT) {
            current_token = get_next_token();
            if (parse_statements() == false) {
                return false;
            }
            if (current_token.type == TOKEN_DEDENT) {
                current_token = get_next_token();
                return true;
            }
        }
        return false;
    }
    else {
        if (parse_statement() == false) {
        return false;
        }
        if (current_token.type == TOKEN_EOL) {
            current_token = get_next_token();
            return true;
        }
        return false;
    }
}

bool parse_statements() {
    while (current_token.type != TOKEN_DEDENT && current_token.type != TOKEN_EOF) {
        if (parse_statement() == false) {
            return false;
        }
        if (current_token.type == TOKEN_EOL) {
            current_token = get_next_token();
        }
        else {
            return false;
        }
    }
    return true;
}

bool parse_statement() {
    if (current_token.type == TOKEN_RETURN) {
        current_token = get_next_token();
        if (parse_expr_or_call_opt() == false) {
            return false;
        }
        return true;
    }
    else if (current_token.type == TOKEN_WHILE) {
        current_token = get_next_token();
        if (parse_expression() == false) {
            return false;
        }
        if (current_token.type == TOKEN_DO) {
            current_token = get_next_token();
            if (current_token.type == TOKEN_EOL) {
                current_token = get_next_token();
                if (current_token.type == TOKEN_INDENT) {
                    current_token = get_next_token();
                    if (parse_statements() == false) {
                        return false;
                    }
                    if (current_token.type == TOKEN_DEDENT) {
                        current_token = get_next_token();
                        return true;
                    }
                }
            }
        }
        return false;
    }
    else if (current_token.type == TOKEN_IF) {
        current_token = get_next_token();
        if (parse_expression() == false) {
            return false;
        }
        if (current_token.type == TOKEN_THEN) {
            current_token = get_next_token();
            if (current_token.type == TOKEN_EOL) {
                current_token = get_next_token();
                if (current_token.type == TOKEN_INDENT) {
                    current_token = get_next_token();
                    if (parse_statements() == false) {
                        return false;
                    }
                    if (current_token.type == TOKEN_DEDENT) {
                        current_token = get_next_token();
                        if (current_token.type == TOKEN_ELSE) {
                            current_token = get_next_token();
                            if (current_token.type == TOKEN_EOL) {
                                current_token = get_next_token();
                                if (current_token.type == TOKEN_INDENT) {
                                    current_token = get_next_token();
                                    if (parse_statements() == false) {
                                        return false;
                                    }
                                    if (current_token.type == TOKEN_DEDENT) {
                                        current_token = get_next_token();
                                        return true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return false;
    }
    else if (current_token.type == TOKEN_VAL || current_token.type == TOKEN_VAR) {
        current_token = get_next_token();
        if (current_token.type == TOKEN_IDENTIFIER) {
            current_token = get_next_token();
            if (parse_type_opt() == false) {
                return false;
            }
            if (current_token.type == TOKEN_ASSIGN) {
                current_token = get_next_token();
                if (parse_expr_or_call() == false) {
                    return false;
                }
                return true;
            }
            return false;
        }
        return false;
    }
    else {
        if (parse_expr_or_call() == false) {
            return false;
        }
        return true;
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

bool parse_expr_or_call() {
    return parse_expression();
}

bool parse_expr_or_call_opt() {
    if (current_token.type == TOKEN_EOL || current_token.type == TOKEN_DEDENT || current_token.type == TOKEN_EOF) {
        return true;
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