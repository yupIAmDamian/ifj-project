// Compiler of IFJ26 imperative language
// Author: Dominik Madaj (xmadajd00)


#include "scanner.h"
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

// States of the FSM
typedef enum {
    STATE_COUNT_INDENT,
    STATE_START,
    STATE_ID_OR_KEYWORD,
    STATE_INTEGER,
    STATE_DOUBLE,
    STATE_STRING,
    STATE_LESS_THAN,
    STATE_GREATER_THAN,
    STATE_EQUALS,
    STATE_EXC_MARK,
    STATE_SLASH,
    STATE_COMMENT,
} State;



int current_line = 1;
bool line_start = true; // variable for counting indentation level

// Function to get the next token from the input
Token get_next_token()
{
    Token token;
    token.line = current_line;    
    State state = STATE_START; // Initial state of the FSM
    if (line_start) { // state for counting the indentation level
        state = STATE_COUNT_INDENT;
    }
    int space_count = 0;

    while(1) {
        int character = getchar();

        switch (state) {
            case STATE_COUNT_INDENT:
                if(character == ' ') {
                    space_count++;
                } else if(character == '\t') { // TAB can't be at the start of a non-empty line
                    token.type = TOKEN_ERROR;
                    return token;
                } else if (character == '\n') { // Line is empty, reset the space counter
                    current_line++;
                    space_count = 0;
                } else { // First non-white character in line found
                    line_start = false;
                    ungetc(character, stdin);  // returns the character back into stdin

                    // needs to send indent / dedent tokens
                }

            case STATE_START:
                if (character == EOF) { // End of file
                    token.type = TOKEN_EOF;
                    return token;
                } else if (character == '\n') { // End of line
                    token.type = TOKEN_EOL;
                    current_line++;
                    line_start = true;
                    return token;
                } else if (isspace(character)) continue; // Skip empty space
                else if (isalpha(character) || character == '_') { // IDs / data types / keywords (can't begin with a number)
                    state = STATE_ID_OR_KEYWORD;
                    continue;
                } else if (character == '"') {
                    state = STATE_STRING;
                }
                else if (character == '+') {
                    token.type = TOKEN_PLUS;
                    return token;
                } else if (character == '-') {
                    token.type = TOKEN_MINUS;
                    return token;
                } else if (character == '*') {
                    token.type = TOKEN_MUL;
                    return token;
                } else if (character == '/') {
                    state = STATE_SLASH;
                } else if (character == '<') {
                    state = STATE_LESS_THAN;
                } else if (character == '>') {
                    state = STATE_GREATER_THAN;
                } else if (character == '=') {
                    state = STATE_EQUALS;
                } else if (character == '!') {
                    state = STATE_EXC_MARK;
                } else if (character == '(') {
                    token.type = TOKEN_LPAREN;
                    return token;
                } else if (character == ')') {
                    token.type = TOKEN_RPAREN;
                    return token;
                } else if (character == ':') {
                    token.type = TOKEN_COLON;
                    return token;
                } else if (character == ',') {
                    token.type = TOKEN_COMMA;
                    return token;
                } else if (character == '.') {
                    token.type = TOKEN_DOT;
                    return token;
                } else if (character == '@') {
                    token.type = TOKEN_AT;
                    return token;
                } else {
                    token.type = TOKEN_ERROR;
                    return token;
                }

            case STATE_ID_OR_KEYWORD:
                if (isalpha(character) || character == '_' || isalnum(character))
                { // character needs to be saved
                    continue;
                } else {
                    ungetc(character, stdin); // returns the character back into stdin

                    // needs to differentiate ID / type / keyword and set the token type

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
                }
                return token;
            
            case STATE_GREATER_THAN: // differentiates between '>' and '>='
                if(character == '=') {
                    token.type = TOKEN_GREATER_EQ;
                } else {
                    ungetc(character, stdin); // returns the character back into stdin
                    token.type = TOKEN_GREATER;
                }
                return token;
            
            case STATE_EQUALS: // differentiates between '=' and '=='
                if(character == '=') {
                    token.type = TOKEN_EQ;
                } else {
                    ungetc(character, stdin); // returns the character back into stdin
                    token.type = TOKEN_ASSIGN;
                }
                return token;

            case STATE_EXC_MARK: // detects '!='
                if(character == '=') {
                    token.type = TOKEN_NOT_EQ;
                } else { // what to do in this case? error token?
                    ungetc(character, stdin); // returns the character back into stdin
                }
                return token;

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

}
