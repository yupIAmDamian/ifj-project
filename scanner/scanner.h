// Compiler of IFJ26 imperative language
// Authors: Dominik Madaj (xmadajd00), Michal Kopček (xkopcem00)

#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "../error.h"

// token types
typedef enum {
    TOKEN_EOF,           // end of file (EOF)
    TOKEN_EOL,           // end of line (LF)
    
    // key words
    TOKEN_DEF,
    TOKEN_DO,
    TOKEN_ELSE,
    TOKEN_IF,
    TOKEN_IMPORT,
    TOKEN_RETURN,
    TOKEN_THEN,
    TOKEN_VAL,
    TOKEN_VAR,
    TOKEN_WHILE,
    TOKEN_UNDERSCORE,

    // data types
    TOKEN_TYPE_INT,
    TOKEN_TYPE_DOUBLE,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_UNIT,
    
    // identifiers and literals
    TOKEN_IDENTIFIER,
    TOKEN_INTEGER,
    TOKEN_DOUBLE,
    TOKEN_STRING,
    
    // operators and special characters
    TOKEN_ASSIGN,        // =
    TOKEN_PLUS,          // +
    TOKEN_MINUS,         // -
    TOKEN_MUL,           // *
    TOKEN_DIV,           // /
    TOKEN_LESS,          // <
    TOKEN_LESS_EQ,       // <=
    TOKEN_GREATER,       // >
    TOKEN_GREATER_EQ,    // >=
    TOKEN_EQ,            // ==
    TOKEN_NOT_EQ,        // !=
    TOKEN_LPAREN,        // (
    TOKEN_RPAREN,        // )
    TOKEN_COLON,         // :
    TOKEN_COMMA,         // ,
    TOKEN_DOT,           // .
    TOKEN_AT,            // @
    
    // support for indentation
    TOKEN_INDENT,        // increase indentation level
    TOKEN_DEDENT,        // decrease indentation level
} TokenType;

// structure for a token
typedef struct {
    TokenType type;
    union {
        int integer_value;
        double double_value;
        char *string_value;
    } attribute;
    int line; // line where the token is located (used for error messages)
} Token;

// states of the scanner FSM
typedef enum {
    STATE_INDENT_DEDENT,
    STATE_INDENT_DEDENT_SLASH,
    STATE_START,
    STATE_ID_OR_KEYWORD,
    STATE_INTEGER,
    STATE_INTEGER_UNDERSCORE,
    STATE_ZERO,
    STATE_DOUBLE_DOT,
    STATE_DOUBLE_FRACTION,
    STATE_DOUBLE_FRACTION_UNDERSCORE,
    STATE_DOUBLE_EXP,
    STATE_DOUBLE_EXP_DIGITS_UNDERSCORE,
    STATE_DOUBLE_EXP_SIGN,
    STATE_DOUBLE_EXP_DIGITS,
    STATE_DOUBLE_SUFFIX,
    STATE_DOT,
    STATE_STRING,
    STATE_STRING_QUOTE2,
    STATE_STRING_SINGLE,
    STATE_STRING_SINGLE_ESCAPE,
    STATE_STRING_MULTI,
    STATE_STRING_MULTI_END1,
    STATE_STRING_MULTI_END2,
    STATE_COMMENT_TAIL,
    STATE_LESS_THAN,
    STATE_GREATER_THAN,
    STATE_EQUALS,
    STATE_EXC_MARK,
    STATE_SLASH,
    STATE_COMMENT_SINGLE,
    STATE_COMMENT_BLOCK,
    STATE_COMMENT_BLOCK_END,
    STATE_COMMENT_BLOCK_NESTED,
} State;

Token get_next_token(void);

void scanner_init(void);
void scanner_free(void);
// structure for dynamic string buffer
typedef struct {
    char *str;
    int length;
    int capacity;
} StringKeywordID;

// buffer functions
void buffer_init(StringKeywordID *buffer);
void buffer_add_char(StringKeywordID *buffer, char character);
void buffer_free(StringKeywordID *buffer);

#endif