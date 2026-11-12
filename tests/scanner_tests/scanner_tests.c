// Compiler of IFJ26 imperative language
// Author: Dominik Madaj (xmadajd00)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../scanner/scanner.h"

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
        case TOKEN_ERROR: return "TOKEN_ERROR";
    }
    return "TOKEN_ERROR";
}

// structure for expected tokens
typedef struct {
    TokenType type;
    const char* expected_str;
    int expected_int;
    double expected_double;
} ExpectedToken;


int main() {
    // TEST buffer functions
    printf("\n------Buffer tests------\n");
    StringKeywordID buffer;
    if (!buffer_init(&buffer)) {
        printf("failed to initialize buffer\n");
        return 1;
    }

    for (int i = 0; i < 4; i++) {
        buffer_add_char(&buffer, 'i');
    buffer_add_char(&buffer, 'f');
    buffer_add_char(&buffer, 'j');
    buffer_add_char(&buffer, '-');
    }
    

    if (strcmp(buffer.str, "ifj-ifj-ifj-ifj-") == 0) {
        printf("found expected buffer string: %s\n", buffer.str);
    } else {
        printf("buffer test failed, got '%s'\n", buffer.str);
    }
    
    buffer_free(&buffer);

    //TEST get_next_token
    printf("\n------Test get_next_token------\n");
    
    // sequence of tokens expected to be outputted
    ExpectedToken expected[] = {
        {.type =TOKEN_VAR},
        {.type =TOKEN_IDENTIFIER, .expected_str = "num"},
        {.type =TOKEN_ASSIGN},
        {.type =TOKEN_INTEGER, .expected_int = 5},
        {.type =TOKEN_EOL},
        {.type =TOKEN_IDENTIFIER, .expected_str = "print"},
        {.type =TOKEN_LPAREN},
        {.type =TOKEN_STRING, .expected_str = "\ncycle"},
        {.type =TOKEN_COMMA},
        {.type =TOKEN_STRING, .expected_str = "wait"},
        {.type =TOKEN_RPAREN},
        {.type =TOKEN_EOL},
        {.type =TOKEN_WHILE},
        {.type =TOKEN_IDENTIFIER, .expected_str = "num"},
        {.type =TOKEN_GREATER_EQ},
        {.type =TOKEN_INTEGER, .expected_int = 1},
        {.type =TOKEN_DO},
        {.type =TOKEN_EOL},
        {.type =TOKEN_INDENT},
        {.type =TOKEN_IDENTIFIER, .expected_str = "num"},
        {.type =TOKEN_ASSIGN},
        {.type =TOKEN_IDENTIFIER, .expected_str = "num"},
        {.type =TOKEN_MINUS},
        {.type =TOKEN_INTEGER, .expected_int = 2},
        {.type =TOKEN_EOL},
        {.type =TOKEN_INDENT},
        {.type =TOKEN_IMPORT},
        {.type =TOKEN_IDENTIFIER, .expected_str = "ifj26"},
        {.type =TOKEN_DOT},
        {.type =TOKEN_MUL},
        {.type =TOKEN_EOL},
        {.type =TOKEN_DEDENT},
        {.type =TOKEN_DEDENT},
        {.type =TOKEN_IDENTIFIER, .expected_str = "noindentshere"},
        {.type =TOKEN_EOL},
        {.type =TOKEN_DEF},
        {.type =TOKEN_IDENTIFIER, .expected_str = "abcd"},
        {.type =TOKEN_LPAREN},
        {.type =TOKEN_IDENTIFIER, .expected_str = "n"},
        {.type =TOKEN_COLON},
        {.type =TOKEN_TYPE_INT},
        {.type =TOKEN_RPAREN},
        {.type =TOKEN_COLON},
        {.type =TOKEN_TYPE_DOUBLE},
        {.type =TOKEN_EOL},
        {.type =TOKEN_INDENT},
        {.type =TOKEN_IDENTIFIER, .expected_str = "n"},
        {.type =TOKEN_ASSIGN},
        {.type =TOKEN_IDENTIFIER, .expected_str = "n"},
        {.type =TOKEN_MUL},
        {.type =TOKEN_IDENTIFIER, .expected_str = "n"},
        {.type =TOKEN_EOL},
        {.type =TOKEN_DEDENT},
        {.type =TOKEN_IF},
        {.type =TOKEN_IDENTIFIER, .expected_str = "abcd"},
        {.type =TOKEN_LPAREN},
        {.type =TOKEN_INTEGER, .expected_int = 5},
        {.type =TOKEN_RPAREN},
        {.type =TOKEN_EQ},
        {.type =TOKEN_INTEGER, .expected_int = 25},
        {.type =TOKEN_THEN},
        {.type =TOKEN_EOL},
        {.type =TOKEN_INDENT},
        {.type =TOKEN_STRING, .expected_str = "this string\n is a \"\"multi\\line\" string"},
        {.type =TOKEN_EOL},
        {.type =TOKEN_ELSE},
        {.type =TOKEN_EOL},
        {.type =TOKEN_DEDENT},
        {.type =TOKEN_DOUBLE, .expected_double = 1.564},
        {.type =TOKEN_EOL},
        {.type =TOKEN_MINUS},
        {.type =TOKEN_DOUBLE, .expected_double = 0.942},
        {.type =TOKEN_EOL},
        {.type =TOKEN_VAL},
        {.type =TOKEN_INTEGER, .expected_int = 88},
        {.type =TOKEN_EOL},
        {.type =TOKEN_INDENT},
        {.type =TOKEN_STRING, .expected_str = "inde\nnt"},
        {.type =TOKEN_EOL},
        {.type =TOKEN_STRING, .expected_str = "indent should be the same"},
        {.type =TOKEN_DEDENT},
        {.type =TOKEN_EOF},
    };
    int expected_count = sizeof(expected) / sizeof(expected[0]);
    int current_index = 0;
    int test_passed = 1;

    // this part of the test was mostly generated by AI (after many iterations)
    /* Prompt: "Write a simple test for my scanner. It should call the get_next_token
    function, check if each returned token is correct using if-conditons, check the int/double/string values in
    the token, and keep looping until it reaches EOF." */
    Token token; 
    do {
        token = get_next_token();
        if (current_index >= expected_count) {
            printf("UNEXPECTED TOKEN:  %s\n", token_type_to_string(token.type));
            test_passed = 0;
            break;
        }

        ExpectedToken exp = expected[current_index];
        printf("Token %d: %-25s ", current_index + 1, token_type_to_string(token.type));

        //check token type
        if (token.type != exp.type) {
            printf("\n  EXPECTED: %s\n", token_type_to_string(exp.type));
            test_passed = 0;
        } 
        else { // check token attributes
            if (token.type == TOKEN_IDENTIFIER || token.type == TOKEN_STRING) {
                if (strcmp(token.attribute.string_value, exp.expected_str) == 0) {
                    printf("STR: %s ", token.attribute.string_value);
                } else {
                    printf("\n  EXPECTED: '%s', RETURNED: '%s'", exp.expected_str, token.attribute.string_value);
                    test_passed = 0;
                }
                free(token.attribute.string_value);
            } 
            else if (token.type == TOKEN_INTEGER) {
                if (token.attribute.integer_value == exp.expected_int) {
                    printf("INT: %d ", token.attribute.integer_value);
                } else {
                    printf("\n  EXPECTED: %d, RETURNED: %d", 
                           exp.expected_int, token.attribute.integer_value);
                    test_passed = 0;
                }
            } 
            else if (token.type == TOKEN_DOUBLE) {
                if (token.attribute.double_value == exp.expected_double) {
                    printf("DOUBLE: %f ", token.attribute.double_value);
                } else {
                    printf("\n  EXPECTED: %f, RETURNED: %f", 
                           exp.expected_double, token.attribute.double_value);
                    test_passed = 0;
                }
            } else {
            }
        }
        printf("\n");

        current_index++;
    } while(token.type != TOKEN_EOF);

    if (test_passed && current_index == expected_count) {
        printf("\n---ALL TOKENS ARE CORRECT---\n");
    } else {
        printf("\n---FAILED!---\n");
    }

    return 0;
}