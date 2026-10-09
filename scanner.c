// Compiler of IFJ26 imperative language
// Author: Dominik Madaj (xmadajd00)


#include "scanner.h"
#include <stdio.h>
#include <ctype.h>

// States of the FSM
typedef enum {
    STATE_START,
    STATE_ID_OR_KEYWORD,
    STATE_INTEGER,
    STATE_DOUBLE,
    STATE_STRING,
    STATE_LESS_THAN,
    STATE_GREATER_THAN,
    STATE_EQUALS,
    STATE_SLASH,
    STATE_COMMENT,
} State;



int current_line = 1;

// Function to get the next token from the input
Token get_next_token()
{
    Token token;
    token.line = current_line;
    State state = STATE_START; // Initial state of the FSM

    while(1) {
        int character = getchar();

        switch (state) {
            case STATE_START:
                if (character == EOF) { // End of file
                    token.type = TOKEN_EOF;
                    return token;
                } else if (character == '\n') { // End of line
                    token.type = TOKEN_EOL;
                    current_line++;
                    return token;
                } else if (isspace(character)) continue; //Skip empty space
                else if (isalpha(character) || character == '_') { // IDs and keywords (can't begin with a number)
                    state = STATE_ID_OR_KEYWORD;
                    continue;
                } else if (character == '<') {
                    state = STATE_LESS_THAN;
                } else if (character == '>') {
                    state = STATE_GREATER_THAN;
                } else if (character == '=') {
                    state = STATE_EQUALS;
                }
                // need to add all the other possible conditions here

            case STATE_ID_OR_KEYWORD:
                if (isalpha(character) || character == '_' || isalnum(character))
                { // character needs to be saved
                    continue;
                }
                 else { // needs to differentiate ID / keyword and set the token type
                    ungetc(character, stdin); // returns the character back into stdin
                    return token;
                }
            
            case STATE_INTEGER:
                continue;
            
            case STATE_DOUBLE:
                continue;
            
            case STATE_STRING:
                continue;
            
            case STATE_LESS_THAN: // differentiates between '<' and '<='
                if(character == '=') {
                    token.type = TOKEN_LESS_EQ;
                } else {
                    ungetc(character, stdin); // returns the character back into stdin
                    token.type = TOKEN_LESS;
                    return token;
                }
            
            case STATE_GREATER_THAN: // differentiates between '>' and '>='
                if(character == '=') {
                    token.type = TOKEN_GREATER_EQ;
                } else {
                    ungetc(character, stdin); // returns the character back into stdin
                    token.type = TOKEN_GREATER;
                    return token;
                }
            
            case STATE_EQUALS: // differentiates between '=' and '=='
                if(character == '=') {
                    token.type = TOKEN_EQ;
                } else {
                    ungetc(character, stdin); // returns the character back into stdin
                    token.type = TOKEN_ASSIGN;
                    return token;
                }

            case STATE_SLASH: // differentiates between '/' and '//'
                if(character == '/') {
                    state = STATE_COMMENT;
                } else {
                    ungetc(character, stdin); // returns the character back into stdin
                    token.type = TOKEN_DIV;
                    return token;
                }
            
            case STATE_COMMENT: // throws away all comments
                if(character != '\n') {
                    continue;
                } else {
                    ungetc(character, stdin); // returns the character back into stdin
                }

        }
    
    }

    return token;

}
