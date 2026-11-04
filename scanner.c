// Compiler of IFJ26 imperative language
// Author: Dominik Madaj (xmadajd00)


#include "scanner.h"
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

int current_line;
bool line_start; // variable for counting indentation level

//Indentation stack suggested by AI
/*Prompt: "What is the best way for the lexical scanner to keep track of indentation in the input code?
should i have a variable for counting whitespace characters at the beginning of each line or should i send
every whitespace as an indent token? what is the simplest implementation?"*/
#define MAX_INDENT_LEVELS 100 // max indentation level - can it be defined this way??
int indent_stack[MAX_INDENT_LEVELS];
int indent_top;
int pending_dedents;

void scanner_init() {
    current_line = 1;
    line_start = true;
    indent_top = 0;
    indent_stack[indent_top] = 0; // Stack is at 0
    pending_dedents = 0;
}

// Function to get the next token from the input
Token get_next_token()
{
    Token token;
    token.line = current_line;    
    State state = STATE_START; // Initial state of the FSM

    if (pending_dedents > 0) { // Processing of pending dedents
        pending_dedents--;
        token.type = TOKEN_DEDENT;
        return token;
    }

    if (line_start) { // State for counting the indentation level
        state = STATE_INDENT_DEDENT;
    }

    int space_count = 0;

    while(1) {
        int character = getchar();

        switch (state) {
            case STATE_INDENT_DEDENT:
                if(character == ' ') {
                    space_count++;
                } else if(character == '\t') { // TAB can't be at the start of a non-empty line
                    token.type = TOKEN_ERROR;
                    return token;
                } else if (character == '\n') { // Line is empty, reset the space counter
                    current_line++;
                    space_count = 0;
                } else if (character == EOF) { // End of file, dedents must be returned
                    if (indent_top > 0) {
                        pending_dedents = indent_top;
                        indent_top = 0;
                        pending_dedents--;
                        token.type = TOKEN_DEDENT;
                        return token;
                    } else {
                        token.type = TOKEN_EOF;
                        return token;
                    }
                } else { // First non-white character in line found
                    line_start = false;
                    ungetc(character, stdin);  // returns the character back into stdin
                    int current_indent = indent_stack[indent_top];
                    if (space_count > current_indent) { // Higher indentation level in current line
                        indent_top++;
                        indent_stack[indent_top] = space_count;
                        token.type = TOKEN_INDENT;
                        return token;
                    } else if (space_count < current_indent) { // Lower indentation level in current line
                        while (indent_top > 0 && indent_stack[indent_top] > space_count) {
                            indent_top--;
                            pending_dedents++;
                        }
                        // Space count needs to match a previous indentation level, otherwise error
                        if (indent_stack[indent_top] != space_count) {
                             token.type = TOKEN_ERROR;
                             return token;
                        }

                        pending_dedents--;
                        token.type = TOKEN_DEDENT;
                        return token;
                    } else { // Same indentation level in current line
                        state = STATE_START;
                    }
                }
                break;

            case STATE_START:
                if (character == EOF) { // End of file
                    state = STATE_INDENT_DEDENT; // To return possible pending dedents
                    break;
                } else if (character == '\n') { // End of line
                    token.type = TOKEN_EOL;
                    current_line++;
                    line_start = true;
                    return token;
                } else if (isspace(character)) continue; // Skip empty space
                else if (isalpha(character) || character == '_') { // IDs / data types / keywords (can't begin with a number)
                    state = STATE_ID_OR_KEYWORD;
                    ungetc(character, stdin); // or save the character into a buffer here
                    break;
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
                break;

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
                } else {
                    ungetc(character, stdin); // returns the character back into stdin
                    token.type = TOKEN_ERROR;
                    return token;
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
                } else if (character == EOF) {
                    state = STATE_INDENT_DEDENT;
                    break;
                } else {
                    ungetc(character, stdin); // returns the character back into stdin
                    state = STATE_START;
                    break;
                }

        }
    
    }

}
