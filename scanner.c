// Compiler of IFJ26 imperative language
// Author: Dominik Madaj (xmadajd00)


#include "scanner.h"
#include <stdio.h>

// States of the FSM
typedef enum {
    STATE_START,
    STATE_ID_OR_KEYWORD,
    STATE_INTEGER,
    STATE_DOUBLE,
    STATE_STRING,
    STATE_LESS_THAN,
    STATE_MORE_THAN,
    STATE_COMMENT,
} State;



int current_line = 1;

// Function to get the next token from the input
Token get_next_token()
{
    Token token;
    token.line = current_line;




    return token;

}
