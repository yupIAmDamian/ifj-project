// Compiler of IFJ26 imperative language
// Author: Dominik Madaj (xmadajd00)
#ifndef SCANNER_TESTS_H
#define SCANNER_TESTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../scanner/scanner.h"
#include "../../helper_func.h"
#include "../../error.h"

const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_EOF: return "TOKEN_EOF";
        case TOKEN_EOL: return "TOKEN_EOL";
        case TOKEN_DEF: return "TOKEN_DEF";
        case TOKEN_DO: return "TOKEN_DO";
        case TOKEN_ELSE: return "TOKEN_ELSE";
        case TOKEN_IF: return "TOKEN_IF";
        case TOKEN_IMPORT: return "TOKEN_IMPORT";
        case TOKEN_RETURN: return "TOKEN_RETURN";
        case TOKEN_THEN: return "TOKEN_THEN";
        case TOKEN_VAL: return "TOKEN_VAL";
        case TOKEN_VAR: return "TOKEN_VAR";
        case TOKEN_WHILE: return "TOKEN_WHILE";
        case TOKEN_TYPE_INT: return "TOKEN_TYPE_INT";
        case TOKEN_TYPE_DOUBLE: return "TOKEN_TYPE_DOUBLE";
        case TOKEN_TYPE_STRING: return "TOKEN_TYPE_STRING";
        case TOKEN_TYPE_UNIT: return "TOKEN_TYPE_UNIT";
        case TOKEN_IDENTIFIER: return "TOKEN_IDENTIFIER";
        case TOKEN_INTEGER: return "TOKEN_INTEGER";
        case TOKEN_DOUBLE: return "TOKEN_DOUBLE";
        case TOKEN_STRING: return "TOKEN_STRING";
        case TOKEN_ASSIGN: return "TOKEN_ASSIGN";
        case TOKEN_PLUS: return "TOKEN_PLUS";
        case TOKEN_MINUS: return "TOKEN_MINUS";
        case TOKEN_MUL: return "TOKEN_MUL";
        case TOKEN_DIV: return "TOKEN_DIV";
        case TOKEN_LESS: return "TOKEN_LESS";
        case TOKEN_LESS_EQ: return "TOKEN_LESS_EQ";
        case TOKEN_GREATER: return "TOKEN_GREATER";
        case TOKEN_GREATER_EQ: return "TOKEN_GREATER_EQ";
        case TOKEN_EQ: return "TOKEN_EQ";
        case TOKEN_NOT_EQ: return "TOKEN_NOT_EQ";
        case TOKEN_LPAREN: return "TOKEN_LPAREN";
        case TOKEN_RPAREN: return "TOKEN_RPAREN";
        case TOKEN_COLON: return "TOKEN_COLON";
        case TOKEN_COMMA: return "TOKEN_COMMA";
        case TOKEN_DOT: return "TOKEN_DOT";
        case TOKEN_AT: return "TOKEN_AT";
        case TOKEN_INDENT: return "TOKEN_INDENT";
        case TOKEN_DEDENT: return "TOKEN_DEDENT";
        case TOKEN_UNDERSCORE: return "TOKEN_UNDERSCORE";
        default: return "UNKNOWN TOKEN";
    }
}

#endif