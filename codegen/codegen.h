#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include <string.h>

/*
CHYBOVE HLASKY

Pokus o práci s neexistující proměnnou (čtení nebo zápis) vede na chybu 54. Pokus o čtení hodnoty
neinicializované proměnné vede na chybu 56. Pokus o interpretaci instrukce s operandy nevhod-
ných typů dle popisu dané instrukce vede na chybu 53.

POPFRAME - ak nie je k dispozícii rámec, tak chyba 55. 
PUSHFRAME - POKUS O PRISTUP K NEDEFIN RAMCU - CHYBA 55 
RETURN - instrukcia pri prazdnom zasobniku
*/


void create_head();

void gen_node(ASTNode *root);

void function_call(ASTNode *root);

void function_def(ASTNode *root);

//mozno tieto dve funckie zlucit????
char *data_type(ASTNode *root);//do they save the data type in the AST? if yes, then we can use it to generate code for the data type

char *variable_gen(ASTNode *root);

#endif

/*
MOVE
CREATEFRAME
PUSHFRAME - 
POPFRAME
DEFVAR
CALL {label}
RETURN 

DATOVY ZASOBNIK
PUSHS
POPS
CLEARS

ARITMETICKE A RELACNE OPERACE
ADD
SUB
MUL
DIV
IDIV
ADDS/SUBS/MULS/DIVS/IDIVS - ZASOBNIKOVE VERZIE
LT/GT/EQ
LTS/GTS/EQS - ZASOBNIKOVE VERZIE
AND/OR/NOT
ANDS/ORS/NOTS - ZASOBNIKOVE VERZIE
INT2FLOAT
FLOAT2INT
INT2CHAR
STRI2INT
FLOAT2STR
INT2STR

INT2FLOATS/FLOAT2INTS/INT2CHARS/STRI2INTS/FLOAT2STRS/INT2STRS - ZASOBNIKOVE VERZIE



VSTUPNE VYSTUPNE OPERACE
READ
WRITE

PRACE S RETEZCEM
CONCAT
STRLEN
GETCHAR
SETCHAR

PRACE S POLI
NEWARRAY
ARRAYGET
ARRAYSET

PRACA S TYPMI
TYPE
ISINT
TYPES/ISINTS - ZASOBNIKOVE VERZIE

INSTRUKCIE PRE RIADENIE TOKU
LABEL - DEF NAVESTIA
JUMP
JUMPIFEQ
JUMPIFNEQ
JUMPIFEQS/JUMPIFNEQS - ZASOBNIKOVE VERZIE
EXIT - UKONCENIE ITERACIE S NAVRATOVYM KODOM

LADIACE INSTRUKCIE
BREAK - BREAKPOINT
DPRINT - VYPIS HODNOTY OPERANDU NA STDERR
*/