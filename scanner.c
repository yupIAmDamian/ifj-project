// Compiler of IFJ26 imperative language
// Author: Dominik Madaj (xmadajd00)


#include "scanner.h"
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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

// function for the initialization of the scanner
void scanner_init() {
    current_line = 1;
    line_start = true;
    indent_top = 0;
    indent_stack[indent_top] = 0; // stack is at 0
    pending_dedents = 0;
}

// function for the initialization of the buffer
bool buffer_init(StringKeywordID *buffer) {
    buffer->capacity = 16;
    buffer->length = 0;
    buffer->str = malloc(buffer->capacity * sizeof(char));
    if (buffer->str == NULL) {
        return false; // failed to allocate memory
    }
    buffer->str[0] = '\0';
    return true;
}

// function for adding a character to the buffer
bool buffer_add_char(StringKeywordID *buffer, char character) {
    if (buffer->length + 1 >= buffer->capacity) {
        buffer->capacity *= 2; // the capacity doubles if its reached
        char *new_str = realloc(buffer->str, buffer->capacity * sizeof(char));
        if (new_str == NULL) {
            return false; // failed to allocate memory
        }
        buffer->str = new_str;
    }
    buffer->str[buffer->length] = character; // new character inserted
    buffer->length++;
    buffer->str[buffer->length] = '\0';
    return true;
}

// function for freeing the allocated buffer memory
void buffer_free(StringKeywordID *buffer) {
    if (buffer->str != NULL) {
        free(buffer->str);
        buffer->str = NULL;
    }
}

// function to get the next token from the input
Token get_next_token()
{
    Token token;
    token.line = current_line;    
    State state = STATE_START; // initial state of the FSM

    if (pending_dedents > 0) { // processing of pending dedents
        pending_dedents--;
        token.type = TOKEN_DEDENT;
        return token;
    }

    if (line_start) { // state for counting the indentation level
        state = STATE_INDENT_DEDENT;
    }

    int space_count = 0;
    StringKeywordID buffer; // buffer for storing keywords, IDs, strings, ...
    buffer.str = NULL;

    while(1) {
        int character = getchar();

        switch (state) {
            case STATE_INDENT_DEDENT:
                if (character == ' ') {
                    space_count++;
                } else if (character == '\t') { // TAB can't be at the start of a non-empty line
                    token.type = TOKEN_ERROR;
                    return token;
                } else if (character == '\n') { // line is empty, reset the space counter
                    current_line++;
                    space_count = 0;
                } else if (character == EOF) { // end of file, dedents must be returned
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
                } else { // first non-white character in line found
                    line_start = false;
                    ungetc(character, stdin);  // returns the character back into stdin
                    int current_indent = indent_stack[indent_top];
                    if (space_count > current_indent) { // higher indentation level in current line
                        indent_top++;
                        indent_stack[indent_top] = space_count;
                        token.type = TOKEN_INDENT;
                        return token;
                    } else if (space_count < current_indent) { // lower indentation level in current line
                        while (indent_top > 0 && indent_stack[indent_top] > space_count) {
                            indent_top--;
                            pending_dedents++;
                        }
                        // space count needs to match a previous indentation level, otherwise error
                        if (indent_stack[indent_top] != space_count) {
                             token.type = TOKEN_ERROR;
                             return token;
                        }

                        pending_dedents--;
                        token.type = TOKEN_DEDENT;
                        return token;
                    } else { // same indentation level in current line
                        state = STATE_START;
                    }
                }
                break;

            case STATE_START:
                if (character == EOF) { // end of file
                    state = STATE_INDENT_DEDENT; // to return possible pending dedents
                    break;
                } else if (character == '\n') { // end of line
                    token.type = TOKEN_EOL;
                    current_line++;
                    line_start = true;
                    return token;
                } else if (isspace(character)) continue; // skip empty space
                else if (isalpha(character) || character == '_') { // IDs / data types / keywords (can't begin with a number)
                    if (!buffer_init(&buffer)) { // buffer needs to be initialized
                        token.type = TOKEN_ERROR;
                        return token;
                    }
                    if (!buffer_add_char(&buffer, character)) {
                        buffer_free(&buffer);
                        token.type = TOKEN_ERROR;
                        return token;
                    }
                    state = STATE_ID_OR_KEYWORD;
                    break;
                } else if (character == '"') {
                    state = STATE_STRING;
                } else if (character == '+') {
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
                if (isalnum(character) || character == '_')
                {
                    if (!buffer_add_char(&buffer, character)) {
                        buffer_free(&buffer);
                        token.type = TOKEN_ERROR;
                        return token;
                    }
                } else {
                    ungetc(character, stdin); // returns the character back into stdin
                    // compares the buffer to all keywords
                    if (strcmp(buffer.str, "def") == 0) {
                        token.type = TOKEN_DEF;
                        buffer_free(&buffer);
                    } else if (strcmp(buffer.str, "do") == 0) {
                        token.type = TOKEN_DO;
                        buffer_free(&buffer);
                    } else if (strcmp(buffer.str, "Double") == 0) {
                        token.type = TOKEN_TYPE_DOUBLE;
                        buffer_free(&buffer);
                    } else if (strcmp(buffer.str, "else") == 0) {
                        token.type = TOKEN_ELSE;
                        buffer_free(&buffer);
                    } else if (strcmp(buffer.str, "if") == 0) {
                        token.type = TOKEN_IF;
                        buffer_free(&buffer);
                    } else if (strcmp(buffer.str, "import") == 0) {
                        token.type = TOKEN_IMPORT;
                        buffer_free(&buffer);
                    } else if (strcmp(buffer.str, "Int") == 0) {
                        token.type = TOKEN_TYPE_INT;
                        buffer_free(&buffer);
                    } else if (strcmp(buffer.str, "return") == 0) {
                        token.type = TOKEN_RETURN;
                        buffer_free(&buffer);
                    } else if (strcmp(buffer.str, "String") == 0) {
                        token.type = TOKEN_TYPE_STRING;
                        buffer_free(&buffer);
                    } else if (strcmp(buffer.str, "then") == 0) {
                        token.type = TOKEN_THEN;
                        buffer_free(&buffer);
                    } else if (strcmp(buffer.str, "Unit") == 0) {
                        token.type = TOKEN_TYPE_UNIT;
                        buffer_free(&buffer);
                    } else if (strcmp(buffer.str, "val") == 0) {
                        token.type = TOKEN_VAL;
                        buffer_free(&buffer);
                    } else if (strcmp(buffer.str, "var") == 0) {
                        token.type = TOKEN_VAR;
                        buffer_free(&buffer);
                    } else if (strcmp(buffer.str, "while") == 0) {
                        token.type = TOKEN_WHILE;
                        buffer_free(&buffer);
                    } else { // no keyword matched, its an ID
                        token.type = TOKEN_IDENTIFIER;
                        token.attribute.string_value = buffer.str;
                    }
                    return token;
                }
            
            case STATE_INTEGER:
                continue;
            
            case STATE_DOUBLE:
                continue;
            
            case STATE_STRING:
                continue;
            
            case STATE_LESS_THAN: // differentiates between '<' and '<='
                if (character == '=') {
                    token.type = TOKEN_LESS_EQ;
                } else {
                    ungetc(character, stdin); // returns the character back into stdin
                    token.type = TOKEN_LESS;
                }
                return token;
            
            case STATE_GREATER_THAN: // differentiates between '>' and '>='
                if (character == '=') {
                    token.type = TOKEN_GREATER_EQ;
                } else {
                    ungetc(character, stdin); // returns the character back into stdin
                    token.type = TOKEN_GREATER;
                }
                return token;
            
            case STATE_EQUALS: // differentiates between '=' and '=='
                if (character == '=') {
                    token.type = TOKEN_EQ;
                } else {
                    ungetc(character, stdin); // returns the character back into stdin
                    token.type = TOKEN_ASSIGN;
                }
                return token;

            case STATE_EXC_MARK: // detects '!='
                if (character == '=') {
                    token.type = TOKEN_NOT_EQ;
                } else {
                    ungetc(character, stdin); // returns the character back into stdin
                    token.type = TOKEN_ERROR;
                    return token;
                }
                return token;

            case STATE_SLASH: // differentiates between '/' and '//'
                if (character == '/') {
                    state = STATE_COMMENT;
                } else {
                    ungetc(character, stdin); // returns the character back into stdin
                    token.type = TOKEN_DIV;
                    return token;
                }
            
            case STATE_COMMENT: // throws away all comments
                if (character != '\n') {
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
