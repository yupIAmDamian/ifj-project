#ifndef SCANNER_H
#define SCANNER_H

// Typy tokenov pre lexikalny analyzator a syntakticky analyzator
typedef enum {
    TOKEN_EOF,           // Koniec suboru (EOF)
    TOKEN_EOL,           // Koniec riadku (LF) - dolezite kvoli syntaxi s odsadenim
    
    // Klucove slova (zakladne zadanie)
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
    
    // Datove typy
    TOKEN_TYPE_INT,
    TOKEN_TYPE_DOUBLE,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_UNIT,
    // TOKEN_TYPE_BOOLEAN (ak budeme robit rozsirenie BOOLTHEN)
    
    // Identifikatory a literaly
    TOKEN_IDENTIFIER,
    TOKEN_INTEGER,
    TOKEN_DOUBLE,
    TOKEN_STRING,
    TOKEN_UNIT,          // Literal ()
    
    // Operatory a specialne znaky
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
    TOKEN_DOT,           // . (pre rozsirenia)
    TOKEN_AT,            // @ (pre anotaciu @main)
    
    // Podpora pre odsadenie (indentation)
    TOKEN_INDENT,        // Zvysenie urovne odsadenia
    TOKEN_DEDENT,        // Znizenie urovne odsadenia
    
    // Pripadne chybovy token z lexera
    TOKEN_ERROR
} TokenType;

// Struktura pre Token
typedef struct {
    TokenType type;
    union {
        int integer_value;
        double double_value;
        char *string_value;
        // Mozeme dat dalsie atributy ak bude treba
    } attribute;
    int line; // Riadok kde sa token nachadza (zide sa pri chybovych hlaseniach)
} Token;

// (tato funkcia bude tvoja Dominik a mohla by napriklad brat ukazovatel na subor alebo buffer)
Token get_next_token();

#endif 