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

// helper function for checking buffer initialization errors
Token error_buffer_init() {
    Token token;
    token.type = TOKEN_ERROR;
    return token;
}

// helper function for checking errors when adding a character to the buffer
Token error_buffer_add_char(StringKeywordID *buffer) {
    buffer_free(buffer);
    Token token;
    token.type = TOKEN_ERROR;
    return token;
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
    StringKeywordID buffer; // buffer for storing keywords, IDs, strings, digits, ...
    buffer.str = NULL;
    int comment_nested_count = 0; // counter for nested block comments

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
                } else if (isspace(character)) { // skip empty space
                    break;
                } else if (isalpha(character) || character == '_') { // IDs / data types / keywords (can't begin with a number)
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
                } else if (isdigit(character)) { // numbers
                    if (!buffer_init(&buffer)) {
                        token.type = TOKEN_ERROR;
                        return token;
                    }
                    buffer_add_char(&buffer, character);
                    state = STATE_INTEGER;
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
                    state = STATE_DOT;
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
                        return error_buffer_add_char(&buffer);
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
                if (isdigit(character)) {
                    if(!buffer_add_char(&buffer, character)) {
                        return error_buffer_add_char(&buffer);
                    }
                } else if (character == '_') { // underscores in int/double are ignored
                    break;
                } else if (character == '.') {
                    if (!buffer_add_char(&buffer, character)) {
                        return error_buffer_add_char(&buffer);
                    }
                    state = STATE_DOUBLE_DOT;
                } else if (character == 'e' || character == 'E') {
                    if (!buffer_add_char(&buffer, character)) {
                        return error_buffer_add_char(&buffer);
                    }
                    state = STATE_DOUBLE_EXP;
                } else if (character == 'd' || character == 'D') {
                    state = STATE_DOUBLE_SUFFIX;
                } else if (isalpha(character)) { // no other alphabetical characters are valid here
                    buffer_free(&buffer);
                    token.type = TOKEN_ERROR;
                    return token;
                } else {
                    ungetc(character, stdin);
                    token.type = TOKEN_INTEGER;
                    // conversion of the string in buffer to an int value
                    token.attribute.integer_value =(int)strtol(buffer.str, NULL, 10);
                    buffer_free(&buffer);
                    return token;
                }
                break;

            case STATE_DOT: // differentiates between a regular dot and a double starting with a dot
                if (isdigit(character)) {
                    if (!buffer_init(&buffer)) {
                        return error_buffer_init();
                    }
                    if (!buffer_add_char(&buffer, '.') || !buffer_add_char(&buffer, character)) {
                        error_buffer_add_char(&buffer);
                    }
                    state = STATE_DOUBLE_FRACTION;
                } else {
                    ungetc(character, stdin);
                    token.type = TOKEN_DOT;
                    return token;
                }
                break;

            case STATE_DOUBLE_DOT:
                if (isdigit(character)) { // dot must be followed by a digit
                    if (!buffer_add_char(&buffer, character)) {
                        return error_buffer_add_char(&buffer);
                    }
                    state = STATE_DOUBLE_FRACTION;
                } else {
                    buffer_free(&buffer);
                    token.type = TOKEN_ERROR; 
                    return token;
                }
                break;

            case STATE_DOUBLE_FRACTION:
                if (isdigit(character)) {
                    if (!buffer_add_char(&buffer, character)) {
                        return error_buffer_add_char(&buffer);
                    }
                } else if (character == '_') { // underscores in int/double are ignored
                    break;
                } else if (character == 'e' || character == 'E') {
                    if (!buffer_add_char(&buffer, character)) {
                        return error_buffer_add_char(&buffer);
                    }
                    state = STATE_DOUBLE_EXP;
                } else if (character == 'd' || character == 'D') {
                    state = STATE_DOUBLE_SUFFIX;
                } else if (isalpha(character)) { // no other alphabetical characters are valid here
                    buffer_free(&buffer);
                    token.type = TOKEN_ERROR;
                    return token;
                } else {
                    ungetc(character, stdin);
                    token.type = TOKEN_DOUBLE;
                    token.attribute.double_value = strtod(buffer.str, NULL);
                    buffer_free(&buffer);
                    return token;
                }
                break;

            case STATE_DOUBLE_EXP: // at least one digit must follow the e/E character
                if (character == '+' || character == '-') {
                    if (!buffer_add_char(&buffer, character)) {
                        return error_buffer_add_char(&buffer);
                    }
                    state = STATE_DOUBLE_EXP_SIGN;
                } else if (isdigit(character)) {
                    if (!buffer_add_char(&buffer, character)) {
                        return error_buffer_add_char(&buffer);
                    }
                    state = STATE_DOUBLE_EXP_DIGITS;
                } else {
                    buffer_free(&buffer);
                    token.type = TOKEN_ERROR;
                    return token;
                }
                break;

            case STATE_DOUBLE_EXP_SIGN: // at least one digit must follow the +/- sign in the exponent
                if (isdigit(character)) {
                    if (!buffer_add_char(&buffer, character)) {
                        return error_buffer_add_char(&buffer);
                    }
                    state = STATE_DOUBLE_EXP_DIGITS;
                } else {
                    buffer_free(&buffer);
                    token.type = TOKEN_ERROR;
                    return token;
                }
                break;

            case STATE_DOUBLE_EXP_DIGITS:
                if (isdigit(character)) {
                    if (!buffer_add_char(&buffer, character)) {
                        return error_buffer_add_char(&buffer);
                    }
                } else if (character == '_') {
                    break;
                } else if (character == 'd' || character == 'D') {
                    state = STATE_DOUBLE_SUFFIX;
                } else if (isalpha(character)) { // no other suffixes supported
                    buffer_free(&buffer);
                    token.type = TOKEN_ERROR;
                    return token;
                } else {
                    ungetc(character, stdin);
                    token.type = TOKEN_DOUBLE;
                    token.attribute.double_value = strtod(buffer.str, NULL);
                    buffer_free(&buffer);
                    return token;
                }
                break;

            case STATE_DOUBLE_SUFFIX:
                ungetc(character, stdin);
                token.type = TOKEN_DOUBLE;
                token.attribute.double_value = strtod(buffer.str, NULL);
                buffer_free(&buffer);
                return token;

            case STATE_STRING: // one quot. mark detected
                if (!buffer_init(&buffer)) {
                    return error_buffer_init();
                }
                if (character == '"') {
                    state = STATE_STRING_QUOTE2;
                } else {
                    ungetc(character, stdin);
                    state = STATE_STRING_SINGLE;
                }
                break;

            case STATE_STRING_QUOTE2: // two quot. marks detected
                if (character == '"') { // 3 quot. marks, its a multiline string
                    state = STATE_STRING_MULTI;
                } else { // empty singleline string
                    ungetc(character, stdin);
                    token.type = TOKEN_STRING;
                    token.attribute.string_value = buffer.str;
                    return token;
                }
                break;

            case STATE_STRING_SINGLE: // the string ends when another quot. mark is detected
                if (character == '"') {
                    token.type = TOKEN_STRING;
                    token.attribute.string_value = buffer.str;
                    return token;
                } else if (character == '\\') { // escape sequence
                    state = STATE_STRING_SINGLE_ESCAPE;
                } else if (character == '\n' || character == EOF){ // string doesnt have the ending quot. mark
                    buffer_free(&buffer);
                    token.type = TOKEN_ERROR;
                    return token;
                } else {
                    if (!buffer_add_char(&buffer, character)) {
                        return error_buffer_add_char(&buffer);
                    }
                }
                break;

            case STATE_STRING_SINGLE_ESCAPE:
                if (character == '\\') {
                    if (!buffer_add_char(&buffer, '\\')) {
                        return error_buffer_add_char(&buffer);
                    }
                } else if (character == '"') {
                    if (!buffer_add_char(&buffer, '"')) {
                        return error_buffer_add_char(&buffer);
                    }
                } else if (character == 'n') {
                    if (!buffer_add_char(&buffer, '\n')) {
                        return error_buffer_add_char(&buffer);
                    }
                } else if (character == 'r') {
                    if (!buffer_add_char(&buffer, '\r')) {
                        return error_buffer_add_char(&buffer);
                    }
                } else if (character == 't') {
                    if (!buffer_add_char(&buffer, '\t')) {
                        return error_buffer_add_char(&buffer);
                    }
                } else { // no other escape sequences are valid
                    buffer_free(&buffer);
                    token.type = TOKEN_ERROR;
                    return token;
                }
                state = STATE_STRING_SINGLE;
                break;

            case STATE_STRING_MULTI:
                if (character == '"') {
                    state = STATE_STRING_MULTI_END1;
                } else if (character == EOF) { // string doesnt have the ending quot. marks
                    buffer_free(&buffer);
                    token.type = TOKEN_ERROR;
                    return token;
                } else {
                    if (character == '\n') {
                        current_line++; 
                    }
                    if (!buffer_add_char(&buffer, character)) {
                        return error_buffer_add_char(&buffer);
                    }
                }
                break;

            case STATE_STRING_MULTI_END1:
                if (character == '"') {
                    state = STATE_STRING_MULTI_END2;
                } else { // adds quot. mark to buffer and goes back to the main multiline string state
                    if (!buffer_add_char(&buffer, '"')) {
                        return error_buffer_add_char(&buffer);
                    }
                    ungetc(character, stdin);
                    state = STATE_STRING_MULTI;
                }
                break;

            case STATE_STRING_MULTI_END2:
                if (character == '"') {
                    token.type = TOKEN_STRING;
                    token.attribute.string_value = buffer.str;
                    return token;
                } else { // adds 2 quot. marks to buffer and goes back to the main multiline string state
                    if (!buffer_add_char(&buffer, '"') || !buffer_add_char(&buffer, '"')) {
                        return error_buffer_add_char(&buffer);
                    }
                    ungetc(character, stdin);
                    state = STATE_STRING_MULTI;
                }
                break;

            case STATE_LESS_THAN: // differentiates between '<' and '<='
                if (character == '=') {
                    token.type = TOKEN_LESS_EQ;
                } else {
                    ungetc(character, stdin);
                    token.type = TOKEN_LESS;
                }
                return token;
            
            case STATE_GREATER_THAN: // differentiates between '>' and '>='
                if (character == '=') {
                    token.type = TOKEN_GREATER_EQ;
                } else {
                    ungetc(character, stdin);
                    token.type = TOKEN_GREATER;
                }
                return token;
            
            case STATE_EQUALS: // differentiates between '=' and '=='
                if (character == '=') {
                    token.type = TOKEN_EQ;
                } else {
                    ungetc(character, stdin);
                    token.type = TOKEN_ASSIGN;
                }
                return token;

            case STATE_EXC_MARK: // detects '!='
                if (character == '=') {
                    token.type = TOKEN_NOT_EQ;
                } else {
                    ungetc(character, stdin);
                    token.type = TOKEN_ERROR;
                    return token;
                }
                return token;

            case STATE_SLASH: // differentiates between the div operator, singleline and block comment
                if (character == '/') { // singleline
                    state = STATE_COMMENT_SINGLE;
                } else if (character == '*') { // block
                    comment_nested_count = 1;
                    state = STATE_COMMENT_BLOCK;
                } else {
                    ungetc(character, stdin);
                    token.type = TOKEN_DIV;
                    return token;
                }
                break;
            
            case STATE_COMMENT_SINGLE: // handles singleline comments
                if (character == '\n' || character == EOF) {
                    ungetc(character, stdin);
                    state = STATE_START;
                }
                break;

            case STATE_COMMENT_BLOCK: // handles block comments
                if (character == '*') {
                    state = STATE_COMMENT_BLOCK_END;
                } else if (character == '/') { // slash, possibly a nested block comment
                    state = STATE_COMMENT_BLOCK_NESTED;
                } else if (character == '\n') {
                    current_line++;
                } else if (character == EOF) { // block comment isnt closed with "*/"
                    token.type = TOKEN_ERROR; 
                    return token;
                }
                break;

            case STATE_COMMENT_BLOCK_NESTED:
                if (character == '*') { // beginning of new nested block comment 
                    comment_nested_count++;
                    state = STATE_COMMENT_BLOCK;
                } else if (character == '/') {
                    break;
                } else if (character == '\n') {
                    current_line++;
                    state = STATE_COMMENT_BLOCK;
                } else if (character == EOF) { // block comment isnt closed with "*/"
                    token.type = TOKEN_ERROR;
                    return token;
                } else {
                    state = STATE_COMMENT_BLOCK;
                }
                break;

            case STATE_COMMENT_BLOCK_END:
                if (character == '/') {
                    comment_nested_count--;
                    if (comment_nested_count == 0) { // block comment processing is finished
                        state = STATE_START;
                    } else {
                        state = STATE_COMMENT_BLOCK;
                    }
                } else if (character == '*') {
                    break;
                } else if (character == '\n') {
                    current_line++;
                    state = STATE_COMMENT_BLOCK;
                } else if (character == EOF) { // block comment isnt closed with "*/"
                    token.type = TOKEN_ERROR;
                    return token;
                } else {
                    state = STATE_COMMENT_BLOCK;
                }
                break;

        }
    
    }

}
