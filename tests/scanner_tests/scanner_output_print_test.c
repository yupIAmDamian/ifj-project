// Compiler of IFJ26 imperative language
// Author: Dominik Madaj (xmadajd00)

#include "../../helper_func.h"

void dump_all_tokens(void) {
    printf("--- PRINTING ALL TOKENS ---\n");
    
    Token token;
    int token_count = 0;

    do {
        token = get_next_token();
        token_count++;

        printf("[Line %03d] [%03d] %-12s", token.line, token_count, token_type_name(token.type));

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

    } while (token.type != TOKEN_EOF);

    printf("--- PRINTING FINISHED --- Token count: %d\n", token_count);
}

int main(void) {
    error_register_cleanup(scanner_free);
    scanner_init();
    dump_all_tokens();
    return 0;
}
