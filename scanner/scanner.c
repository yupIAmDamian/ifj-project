// Compiler of IFJ26 imperative language
// Author: Dominik Madaj (xmadajd00)
#include "scanner.h"
#include "helper_func.h"

static int current_line;
static bool line_start; // variable for counting indentation level

//Indentation stack suggested by AI
/*Prompt: "What is the best way for the lexical scanner to keep track of indentation in the input code?
should i have a variable for counting whitespace characters at the beginning of each line or should i send
every whitespace as an indent token? what is the simplest implementation?"*/
#define INIT_MAX_INDENT 20 // initial capacity for indent stack
static int *indent_stack = NULL;
static int indent_capacity = 0;
static int indent_top;
static int pending_dedents;
static bool pending_slash; // flag for putting a slash in the input without using another "ungetc"
static int paren_count; // counter of left parentheses (EOL tokens arent returned inside parentheses)
static TokenType last_token;

// function for the initialization of the indent stack
static void indent_stack_init(void) {
    if (indent_stack == NULL) {
        indent_stack = malloc(INIT_MAX_INDENT * sizeof(int));
        if (indent_stack == NULL) {
            error_exit(ERR_INTERNAL, current_line, "failed to allocate memory for indent stack");
        }
        indent_capacity = INIT_MAX_INDENT;
    }
    indent_top = 0;
    indent_stack[0] = 0;
}

// function for putting an indent level in the stack
static void indent_stack_push(int indent_level) {
    if (indent_top + 1 >= indent_capacity) {
        int new_capacity = indent_capacity * 2; // doubles the capacity
        int *new_stack = realloc(indent_stack, new_capacity * sizeof(int));
        if (new_stack == NULL) {
            error_exit(ERR_INTERNAL, current_line, "failed to reallocate memory for indent stack");
        }
        indent_stack = new_stack;
        indent_capacity = new_capacity;
    }
    indent_top++;
    indent_stack[indent_top] = indent_level;
}

//function fo freeing the allocated stack memory
static void indent_stack_free(void) {
    free(indent_stack);
    indent_stack = NULL;
    indent_capacity = 0;
    indent_top = 0;
}

// function for the initialization of the scanner
void scanner_init(void) {
    current_line = 1;
    line_start = true;
    pending_dedents = 0;
    pending_slash = false;
    paren_count = 0;
    indent_stack_init();
    last_token = TOKEN_EOL;
}

// function for freeing allocated memory when exiting the program
void scanner_free(void) {
    indent_stack_free();
}

// function for the initialization of the buffer
void buffer_init(StringKeywordID *buffer) {
    buffer->capacity = 16;
    buffer->length = 0;
    buffer->str = malloc(buffer->capacity * sizeof(char));
    if (buffer->str == NULL) {
        error_exit(ERR_INTERNAL, current_line, "failed to allocate memory for string buffer in scanner");
    }
    buffer->str[0] = '\0';
}

// function for freeing the allocated buffer memory
void buffer_free(StringKeywordID *buffer) {
    if (buffer->str != NULL) {
        free(buffer->str);
        buffer->str = NULL;
    }
}

// function for adding a character to the buffer
void buffer_add_char(StringKeywordID *buffer, char character) {
    if (buffer->length + 1 >= buffer->capacity) {
        buffer->capacity *= 2; // the capacity doubles if its reached
        char *new_str = realloc(buffer->str, buffer->capacity * sizeof(char));
        if (new_str == NULL) {
            buffer_free(buffer);
            error_exit(ERR_INTERNAL, current_line, "failed to add character to string buffer");
        }
        buffer->str = new_str;
    }
    buffer->str[buffer->length] = character; // new character inserted
    buffer->length++;
    buffer->str[buffer->length] = '\0';
}

// function to get the next token from the input
static Token get_next_token_fsm(void)
{
    Token token;
    token.newline_before = false;
    State state = STATE_START; // initial state of the FSM

    if (pending_dedents > 0) { // processing of pending dedents
        pending_dedents--;
        token.type = TOKEN_DEDENT;
        token.line = current_line;
        return token;
    }

    if (line_start) { // indentation needs to be counted
        state = STATE_INDENT_DEDENT;
    }

    int space_count = 0;
    StringKeywordID buffer; // buffer for storing keywords, IDs, strings, digits, ...
    buffer.str = NULL;
    int comment_nested_count = 0; // counter for nested block comments
    bool found_tab = false; // flag for returning an error if TAB is at the start of a non-empty line
    bool comment_line_start = false;
    bool comment_had_newline = false;
    bool after_comment = false;

    while(1) {
        int character;
        if (pending_slash) {
            character = '/';
            pending_slash = false;
        } else {
            character = getchar();
        }
        bool process_indent_dedent = false; // flag for starting indent and dedent processing

        switch (state) {
            case STATE_INDENT_DEDENT:
                token.line = current_line;
                if (character == ' ') {
                    if (!after_comment) {
                        space_count++;
                    }
                } else if (character == '\t') {
                    if (!after_comment) {
                        found_tab = true; // if the line isnt empty, TAB will result in error
                    }
                } else if (character == '\n') { // line is empty, reset the space counter
                    current_line++;
                    space_count = 0;
                    found_tab = false;
                    after_comment = false;
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
                } else if (character == '/') {
                    state = STATE_INDENT_DEDENT_SLASH;
                } else { // first non-whitespace character in line found
                    ungetc(character, stdin);
                    process_indent_dedent = true; // indent/dedent token will be returned
                }
                break;

            case STATE_INDENT_DEDENT_SLASH:
                if (character == '/') {
                    line_start = false;
                    state = STATE_COMMENT_SINGLE;
                    found_tab = false;
                } else if (character == '*') {
                    comment_line_start = true;
                    comment_nested_count = 1;
                    state = STATE_COMMENT_BLOCK;
                } else { // the slash wasnt the beginning of a comment, indents/dedents need to be returned
                    ungetc(character, stdin);
                    process_indent_dedent = true;
                    pending_slash = true; // the slash will be processed in the following token
                }
                break;

            case STATE_START:
                token.line = current_line;
                if (character == EOF) { // end of file
                    state = STATE_INDENT_DEDENT; // to return possible pending dedents
                    break;
                } else if (character == '\n') { // end of line
                    current_line++;
                    if (paren_count > 0) { // EOL isnt returned inside parentheses
                        token.newline_before = true;
                        break;
                    } else {
                        token.type = TOKEN_EOL;
                        line_start = true;
                        return token;
                    }
                } else if (character == ';') {
                    token.type = TOKEN_EOL; // semicolon means end of logical line
                    return token;
                } else if (isspace(character)) { // skip empty space
                    break;
                } else if (isalpha(character) || character == '_') { // IDs / data types / keywords (can't begin with a number)
                    buffer_init(&buffer);
                    buffer_add_char(&buffer, character);
                    state = STATE_ID_OR_KEYWORD;
                } else if (character == '0') {
                    buffer_init(&buffer);
                    buffer_add_char(&buffer, character);
                    state = STATE_ZERO;
                } else if (isdigit(character)) { // numbers except 0
                    buffer_init(&buffer);
                    buffer_add_char(&buffer, character);
                    state = STATE_INTEGER;
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
                    paren_count++;
                    token.type = TOKEN_LPAREN;
                    return token;
                } else if (character == ')') {
                    if (paren_count > 0) {
                        paren_count--;
                    }
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
                // boolthen extension
                } else if (character == '&') {
                    state = STATE_AMPERSAND;
                } else if (character == '|') {
                    state = STATE_PIPE;
                } else {
                    error_exit(ERR_LEX, current_line, "invalid character on input");
                }
                break;

            case STATE_ID_OR_KEYWORD:
                if (isalnum(character) || character == '_') {
                    buffer_add_char(&buffer, character);
                } else {
                    ungetc(character, stdin); // returns the character back into stdin
                    // compares the buffer to all keywords
                    if (matching_string(buffer.str, "def")) {
                        token.type = TOKEN_DEF;
                    } else if (matching_string(buffer.str, "do")) {
                        token.type = TOKEN_DO;
                    } else if (matching_string(buffer.str, "Double")) {
                        token.type = TOKEN_TYPE_DOUBLE;
                    } else if (matching_string(buffer.str, "else")) {
                        token.type = TOKEN_ELSE;
                    } else if (matching_string(buffer.str, "if")) {
                        token.type = TOKEN_IF;
                    } else if (matching_string(buffer.str, "import")) {
                        token.type = TOKEN_IMPORT;
                    } else if (matching_string(buffer.str, "Int")) {
                        token.type = TOKEN_TYPE_INT;
                    } else if (matching_string(buffer.str, "return")) {
                        token.type = TOKEN_RETURN;
                    } else if (matching_string(buffer.str, "String")) {
                        token.type = TOKEN_TYPE_STRING;
                    } else if (matching_string(buffer.str, "then")) {
                        token.type = TOKEN_THEN;
                    } else if (matching_string(buffer.str, "Unit")) {
                        token.type = TOKEN_TYPE_UNIT;
                    } else if (matching_string(buffer.str, "val")) {
                        token.type = TOKEN_VAL;
                    } else if (matching_string(buffer.str, "var")) {
                        token.type = TOKEN_VAR;
                    } else if (matching_string(buffer.str, "while")) {
                        token.type = TOKEN_WHILE;
                    } else if (matching_string(buffer.str, "_")) {
                        token.type = TOKEN_UNDERSCORE;
                    // boolthen extension
                    } else if (matching_string(buffer.str, "Boolean")) {
                        token.type = TOKEN_TYPE_BOOLEAN;
                    } else if (matching_string(buffer.str, "true")) {
                        token.type = TOKEN_TRUE;
                    } else if (matching_string(buffer.str, "false")) {
                        token.type = TOKEN_FALSE;
                    } else { // no keyword matched, its an ID
                        token.type = TOKEN_IDENTIFIER;
                        token.attribute.string_value = buffer.str;
                        return token;
                    }
                    buffer_free(&buffer);
                    return token;
                }
                break;

            case STATE_INTEGER:
                if (isdigit(character)) {
                    buffer_add_char(&buffer, character);
                } else if (character == '_') {
                    state = STATE_INTEGER_UNDERSCORE;
                } else if (character == '.') {
                    buffer_add_char(&buffer, character);
                    state = STATE_DOUBLE_DOT;
                } else if (character == 'e' || character == 'E') {
                    buffer_add_char(&buffer, character);
                    state = STATE_DOUBLE_EXP;
                } else if (character == 'd' || character == 'D') {
                    state = STATE_DOUBLE_SUFFIX;
                } else {
                    ungetc(character, stdin);
                    token.type = TOKEN_INTEGER;
                    // conversion of the string in buffer to an int value
                    token.attribute.integer_value =(int)strtol(buffer.str, NULL, 10);
                    buffer_free(&buffer);
                    return token;
                }
                break;

            case STATE_INTEGER_UNDERSCORE:
                if (character == '_') {
                    break;
                } else if (isdigit(character)) {
                    buffer_add_char(&buffer, character);
                    state = STATE_INTEGER;
                } else { // underscore cant be at the end of the number
                    buffer_free(&buffer);
                    error_exit(ERR_LEX, current_line, "underscore cant be at the end of a integer");
                }
                break;

            case STATE_ZERO:
                if (character == '.') {
                    buffer_add_char(&buffer, character);
                    state = STATE_DOUBLE_DOT;
                } else if (character == 'e' || character == 'E') {
                    buffer_add_char(&buffer, character);
                    state = STATE_DOUBLE_EXP;
                } else if (character == 'd' || character == 'D') {
                    state = STATE_DOUBLE_SUFFIX;
                } else if (isalnum(character) || character == '_') { // non-zero int cant begin with the digit '0'
                    buffer_free(&buffer);
                    error_exit(ERR_LEX, current_line, "non-zero literal cant begin with the digit 0");
                } else {
                    ungetc(character, stdin);
                    token.type = TOKEN_INTEGER;
                    token.attribute.integer_value = 0;
                    buffer_free(&buffer);
                    return token;
                }
                break;

            case STATE_DOT: // differentiates between a regular dot and a double starting with a dot
                if (isdigit(character)) {
                    buffer_init(&buffer);
                    buffer_add_char(&buffer, '.');
                    buffer_add_char(&buffer, character);
                    state = STATE_DOUBLE_FRACTION;
                } else {
                    ungetc(character, stdin);
                    token.type = TOKEN_DOT;
                    return token;
                }
                break;

            case STATE_DOUBLE_DOT:
                if (isdigit(character)) { // dot must be followed by a digit
                    buffer_add_char(&buffer, character);
                    state = STATE_DOUBLE_FRACTION;
                } else {
                    buffer_free(&buffer);
                    error_exit(ERR_LEX, current_line, "dot inside of a number must be followed by at least 1 digit");
                }
                break;

            case STATE_DOUBLE_FRACTION:
                if (isdigit(character)) {
                    buffer_add_char(&buffer, character);
                } else if (character == '_') {
                    state = STATE_DOUBLE_FRACTION_UNDERSCORE;
                } else if (character == 'e' || character == 'E') {
                    buffer_add_char(&buffer, character);
                    state = STATE_DOUBLE_EXP;
                } else if (character == 'd' || character == 'D') {
                    state = STATE_DOUBLE_SUFFIX;
                } else {
                    ungetc(character, stdin);
                    token.type = TOKEN_DOUBLE;
                    token.attribute.double_value = strtod(buffer.str, NULL);
                    buffer_free(&buffer);
                    return token;
                }
                break;

            case STATE_DOUBLE_FRACTION_UNDERSCORE:
                if (character == '_') {
                    break;
                } else if (isdigit(character)) {
                    buffer_add_char(&buffer, character);
                    state = STATE_DOUBLE_FRACTION;
                } else { // underscore cant be at the end of a fraction
                    buffer_free(&buffer);
                    error_exit(ERR_LEX, current_line, "underscore cant be at the end of a fraction");
                }
                break;

            case STATE_DOUBLE_EXP: // at least one digit must follow the e/E character
                if (character == '+' || character == '-') {
                    buffer_add_char(&buffer, character);
                    state = STATE_DOUBLE_EXP_SIGN;
                } else if (isdigit(character)) {
                    buffer_add_char(&buffer, character);
                    state = STATE_DOUBLE_EXP_DIGITS;
                } else {
                    buffer_free(&buffer);
                    error_exit(ERR_LEX, current_line, "invalid exponent in double");
                }
                break;

            case STATE_DOUBLE_EXP_SIGN: // at least one digit must follow the +/- sign in the exponent
                if (isdigit(character)) {
                    buffer_add_char(&buffer, character);
                    state = STATE_DOUBLE_EXP_DIGITS;
                } else {
                    buffer_free(&buffer);
                    error_exit(ERR_LEX, current_line, "invalid exponent in double");
                }
                break;

            case STATE_DOUBLE_EXP_DIGITS:
                if (isdigit(character)) {
                    buffer_add_char(&buffer, character);
                } else if (character == '_') {
                    state = STATE_DOUBLE_EXP_DIGITS_UNDERSCORE;
                } else if (character == 'd' || character == 'D') {
                    state = STATE_DOUBLE_SUFFIX;
                } else {
                    ungetc(character, stdin);
                    token.type = TOKEN_DOUBLE;
                    token.attribute.double_value = strtod(buffer.str, NULL);
                    buffer_free(&buffer);
                    return token;
                }
                break;

            case STATE_DOUBLE_EXP_DIGITS_UNDERSCORE:
                if (character == '_') {
                    break;
                } else if (isdigit(character)) {
                    buffer_add_char(&buffer, character);
                    state = STATE_DOUBLE_EXP_DIGITS;
                } else { // underscore cant be at the end of the exponent
                    buffer_free(&buffer);
                    error_exit(ERR_LEX, current_line, "underscore cant be at the end of an exponent");
                }
                break;

            case STATE_DOUBLE_SUFFIX:
                if (isalnum(character) || character == '_') { // suffix must be the end of the number
                    buffer_free(&buffer);
                    error_exit(ERR_LEX, current_line, "number cant continue after a suffix");
                }
                ungetc(character, stdin);
                token.type = TOKEN_DOUBLE;
                token.attribute.double_value = strtod(buffer.str, NULL);
                buffer_free(&buffer);
                return token;

            case STATE_STRING: // one quot. mark detected
                buffer_init(&buffer);
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
                    error_exit(ERR_LEX, current_line, "unterminated string");
                } else {
                    buffer_add_char(&buffer, character);
                }
                break;

            case STATE_STRING_SINGLE_ESCAPE:
                if (character == '\\') {
                    buffer_add_char(&buffer, '\\');
                } else if (character == '"') {
                    buffer_add_char(&buffer, '"');
                } else if (character == 'n') {
                    buffer_add_char(&buffer, '\n');
                } else if (character == 'r') {
                    buffer_add_char(&buffer, '\r');
                } else if (character == 't') {
                    buffer_add_char(&buffer, '\t');
                } else { // no other escape sequences are valid
                    buffer_free(&buffer);
                    error_exit(ERR_LEX, current_line, "invalid escape sequence in string");
                }
                state = STATE_STRING_SINGLE;
                break;

            case STATE_STRING_MULTI:
                if (character == '"') {
                    state = STATE_STRING_MULTI_END1;
                } else if (character == EOF) { // string doesnt have the ending quot. marks
                    buffer_free(&buffer);
                    error_exit(ERR_LEX, current_line, "unterminated multiline string");
                } else {
                    if (character == '\n') {
                        current_line++; 
                    }
                    buffer_add_char(&buffer, character);
                }
                break;

            case STATE_STRING_MULTI_END1:
                if (character == '"') {
                    state = STATE_STRING_MULTI_END2;
                } else { // adds quot. mark to buffer and goes back to the main multiline string state
                    buffer_add_char(&buffer, '"');
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
                    buffer_add_char(&buffer, '"');
                    buffer_add_char(&buffer, '"');
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

            case STATE_EXC_MARK: // differentiates between '!' and '!='
                if (character == '=') {
                    token.type = TOKEN_NOT_EQ;
                } else {
                    ungetc(character, stdin);
                    token.type = TOKEN_NOT;
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
                    comment_had_newline = true;
                    current_line++;
                } else if (character == EOF) { // block comment isnt closed with "*/"
                    error_exit(ERR_LEX, current_line, "unterminated block comment");
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
                    comment_had_newline = true;
                    state = STATE_COMMENT_BLOCK;
                } else if (character == EOF) { // block comment isnt closed with "*/"
                    error_exit(ERR_LEX, current_line, "unterminated block comment");
                } else {
                    state = STATE_COMMENT_BLOCK;
                }
                break;

            case STATE_COMMENT_BLOCK_END:
                if (character == '/') {
                    comment_nested_count--;
                    if (comment_nested_count == 0) {
                        if (comment_had_newline && paren_count == 0) {
                            state = STATE_COMMENT_TAIL;
                        } else if (comment_line_start) {
                            state = STATE_INDENT_DEDENT;
                            after_comment = true;
                        } else {
                            state = STATE_START;
                        }
                        comment_line_start = false;
                        comment_had_newline = false;
                    } else {
                        state = STATE_COMMENT_BLOCK;
                    }
                } else if (character == '*') {
                    break;
                } else if (character == '\n') {
                    current_line++;
                    comment_had_newline = true;
                    state = STATE_COMMENT_BLOCK;
                } else if (character == EOF) { // block comment isnt closed with "*/"
                    error_exit(ERR_LEX, current_line, "unterminated block comment");
                } else {
                    state = STATE_COMMENT_BLOCK;
                }
                break;

            case STATE_COMMENT_TAIL: // comment included '\n', EOL must follow
                if (character == ' ' || character == '\t' || character == '\r') {
                    break;
                } else if (character == '\n' || character == EOF) {
                    if (character == '\n') current_line++;
                    token.type = TOKEN_EOL;
                    line_start = true;
                    return token;
                } else {
                    error_exit(ERR_LEX, current_line, "line cant continue after a multiline block comment");
                }

            case STATE_AMPERSAND:
                if (character == '&') {
                    token.type = TOKEN_AND; 
                    return token;
                } else {
                    error_exit(ERR_LEX, current_line, "invalid input - single &");
                }

            case STATE_PIPE:
                if (character == '|') {
                    token.type = TOKEN_OR; 
                    return token;
                } else {
                    error_exit(ERR_LEX, current_line, "invalid input - single |");
                }
        }

        // indent/dedent processing
        if (process_indent_dedent) {
            if (found_tab) {
                error_exit(ERR_LEX, current_line, "tab found at the start of a non-empty line");
            }
            line_start = false;
            int current_indent = indent_stack[indent_top];
            if (space_count > current_indent) { // current line has a higher indentation level
                indent_stack_push(space_count);
                token.type = TOKEN_INDENT;
                return token;
            } else if (space_count < current_indent) { // current line has a lower indentation level
                while (indent_top > 0 && indent_stack[indent_top] > space_count) {
                    indent_top--;
                    pending_dedents++;
                }
                // space count needs to match a previous indentation level, otherwise error
                if (indent_stack[indent_top] != space_count) {
                        error_exit(ERR_LEX, current_line, "invalid indent structure");
                }
                pending_dedents--;
                token.type = TOKEN_DEDENT;
                return token;
            } else { // current line has the same indentation level
                state = STATE_START;
            }
        }
    
    }

}

// calls the fsm function to get a token, filters it 
Token get_next_token(void) {
    Token t;
    while(1) {
        t = get_next_token_fsm();
        if (t.type == TOKEN_EOL && last_token == TOKEN_EOL) { //more than 1 EOL token in succession wont be returned
            continue;
        }
        last_token = t.type;
        return t;
    }
}
