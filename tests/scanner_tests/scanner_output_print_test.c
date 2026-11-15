// Compiler of IFJ26 imperative language
// Author: Dominik Madaj (xmadajd00)

#include "scanner_tests.h"

void dump_all_tokens() {
    printf("--- PRINTING ALL TOKENS ---\n");
    
    Token token;
    int token_count = 0;

    do {
        token = get_next_token();
        token_count++;

        printf("[%03d] %-12s", token_count, token_type_to_string(token.type));

        if (token.type == TOKEN_IDENTIFIER || token.type == TOKEN_STRING) {
            printf(" : %s", token.attribute.string_value);
            free(token.attribute.string_value);
        } 
        else if (token.type == TOKEN_INTEGER) {
            printf(" : %d", token.attribute.integer_value);
        } 
        else if (token.type == TOKEN_DOUBLE) {
            printf(" : %f", token.attribute.double_value);
        }
        
        printf("\n");

        if (token.type == TOKEN_ERROR) {
            printf("!!! LEX. ERROR, PRINTING STOPPED\n");
            break;
        }

    } while (token.type != TOKEN_EOF);

    printf("--- PRINTING FINISHED --- Token count: %d\n", token_count);
}

int main() {
    dump_all_tokens();
    return 0;
}

