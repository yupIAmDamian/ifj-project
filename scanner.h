#ifndef SCANNER_H
#define SCANNER_H

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
    TOKEN_UNIT,          // literal ()
    
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
    
    // error token
    TOKEN_ERROR
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

Token get_next_token();

void scanner_init();

// structure for dynamic string buffer
typedef struct {
    char *str;
    int length;
    int capacity;
} StringKeywordID;

#endif 