// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)

#include "helper_func.h"

char* str_duplicate(char *str) {
    if (str == NULL) {
        return NULL;
    }
    size_t len = strlen(str);
    char *copy = malloc(len + 1);
    if (copy == NULL) {
        return NULL; 
    }
    strcpy(copy, str);

    return copy;
}

FILE *open_file(char *filename, char *mode) {
    FILE *file = fopen(filename, mode);
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        exit(1);
    }
    return file;
}

char *token_type_name(TokenType type) {
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
        default: return "UNKNOWN_TOKEN";
    }
}

bool check_if_string_is_not_in_array(char *str, char **array, size_t array_size) {
    for (size_t i = 0; i < array_size; i++) {
        if (strcmp(str, array[i]) == 0) {
            return false;
        }
    }
    return true;
}