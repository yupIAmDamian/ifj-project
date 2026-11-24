// Compiler of IFJ26 imperative language
// Author: Dominik Madaj (xmadajd00)

#include "../../helper_func.h"

// structure for expected tokens
typedef struct {
    TokenType type;
    const char* expected_str;
    int expected_int;
    double expected_double;
} ExpectedToken;

bool check_token_attribute(Token token, ExpectedToken exp) {
    char act_buf[64], exp_buf[64];
    const char *act_str = act_buf;
    const char *exp_str = exp_buf;
    const char *label = "";
    bool match = true;

    switch (token.type) {
        case TOKEN_IDENTIFIER:
        case TOKEN_STRING:
            label = "STR";
            act_str = token.attribute.string_value;
            exp_str = exp.expected_str;
            match = (strcmp(act_str, exp_str) == 0);
            break;

        case TOKEN_INTEGER:
            label = "INT";
            snprintf(act_buf, sizeof(act_buf), "%d", token.attribute.integer_value);
            snprintf(exp_buf, sizeof(exp_buf), "%d", exp.expected_int);
            match = (token.attribute.integer_value == exp.expected_int);
            break;

        case TOKEN_DOUBLE:
            label = "DOUBLE";
            snprintf(act_buf, sizeof(act_buf), "%f", token.attribute.double_value);
            snprintf(exp_buf, sizeof(exp_buf), "%f", exp.expected_double);
            match = (token.attribute.double_value == exp.expected_double);
            break;

        default:
            return true;
    }
    if (match) {
        printf("%s: %s ", label, act_str);
    } else {
        printf("\n  EXPECTED: '%s', RETURNED: '%s'", exp_str, act_str);
    }
    if (token.type == TOKEN_IDENTIFIER || token.type == TOKEN_STRING) {
        free(token.attribute.string_value);
    }
    return match;
}

int main(void) {
    error_register_cleanup(scanner_free);
    scanner_init();
    // TEST buffer functions
    printf("\n------Buffer tests------\n");
    StringKeywordID buffer;
    buffer_init(&buffer);

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
        {.type =TOKEN_EOL},
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
        // boolean test
        {.type =TOKEN_TYPE_BOOLEAN},
        {.type =TOKEN_IDENTIFIER, .expected_str = "boolext"},
        {.type =TOKEN_ASSIGN},
        {.type =TOKEN_TRUE},
        {.type =TOKEN_EOL},
        {.type =TOKEN_TYPE_BOOLEAN},
        {.type =TOKEN_IDENTIFIER, .expected_str = "boolfalse"},
        {.type =TOKEN_ASSIGN},
        {.type =TOKEN_FALSE},
        {.type =TOKEN_EOL},
        {.type =TOKEN_IF},
        {.type =TOKEN_IDENTIFIER, .expected_str = "a"},
        {.type =TOKEN_AND},
        {.type =TOKEN_IDENTIFIER, .expected_str = "b"},
        {.type =TOKEN_OR},
        {.type =TOKEN_NOT},
        {.type =TOKEN_IDENTIFIER, .expected_str = "c"},
        {.type =TOKEN_EOL},
        {.type =TOKEN_INDENT},
        {.type =TOKEN_RETURN},
        {.type =TOKEN_IDENTIFIER, .expected_str = "result"},
        {.type =TOKEN_EOL},
        {.type =TOKEN_DEDENT},
        // end of boolean test
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

    // this part of the test (and the funciton check_token_attribute) was mostly generated by AI
    // (after many iterations)
    /* Prompt: "Write a simple test for my scanner. It should call the get_next_token
    function, check if each returned token is correct using if-conditons, check the int/double/string values in
    the token, and keep looping until it reaches EOF." */
    Token token; 
    do {
        token = get_next_token();
        if (current_index >= expected_count) {
            printf("UNEXPECTED TOKEN:  %s\n", token_type_name(token.type));
            test_passed = 0;
            break;
        }

        ExpectedToken exp = expected[current_index];
        printf("Token %d: %-25s ", current_index + 1, token_type_name(token.type));

        //check token type
        if (token.type != exp.type) {
            printf("\n  EXPECTED: %s\n", token_type_name(exp.type));
            test_passed = 0;
        } 
        else { // check token attributes
            if (!check_token_attribute(token, exp)) {
                test_passed = 0;
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