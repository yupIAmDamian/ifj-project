#include "parser.h"
#include <stdio.h>

Token current_token;

int parse();

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