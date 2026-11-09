#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "ast.h"
#include <string.h>
#include "parser.h"

/*
CHYBOVE HLASKY

Pokus o práci s neexistující proměnnou (čtení nebo zápis) vede na chybu 54. Pokus o čtení hodnoty
neinicializované proměnné vede na chybu 56. Pokus o interpretaci instrukce s operandy nevhod-
ných typů dle popisu dané instrukce vede na chybu 53.

POPFRAME - ak nie je k dispozícii rámec, tak chyba 55. 
PUSHFRAME - POKUS O PRISTUP K NEDEFIN RAMCU - CHYBA 55 
RETURN - instrukcia pri prazdnom zasobniku
*/

//mozno implementujem 
/*
ADDS/SUBS/MULS/DIVS/IDIVS Zásobníkové verze aritmetických instrukcí
LTS/GTS/EQS Zásobníkové verze relačních instrukcí
ANDS/ORS/NOTS Zásobníkové verze booleovských instrukcí
INT2FLOATS/FLOAT2INTS/
INT2CHARS/STRI2INTS/
FLOAT2STRS/INT2STRS
Zásobníkové verze konverzních instrukcí
TYPES/ISINTS Zásobníkové verze instrukcí pro práci s typy
JUMPIFEQS/JUMPIFNEQS ⟨label⟩ Zásobníkové verze podmíněných skoků
*/

void gen_code(ASTNode *root);

void create_head();

Token get_token_attributes(ASTNode *node);

void name_mangle(char *name, char *mangled_name);

//clears allocated resources
void gen_clear();

void gen_node(ASTNode *root);

void gen_fun_call(ASTNode *root);

void gen_fun_def_start(ASTNode *root);

void gen_fun_def_args(ASTNode *root)

void gen_fun_def_end(ASTNode *root);

void gen_params_declaration(char *param_name);

void gen_while(ASTNode *root);

void gen_if(ASTNode *root);

void gen_if_else(ASTNode *root);

void decide_condition(char *cond_attribute, ASTNode *condition_node);

//mozno tieto dve funckie zlucit????
void gen_data_type(ASTNode *root);//do they save the data type in the AST? if yes, then we can use it to generate code for the data type

void gen_var(ASTNode *root);

//functions for generating code
/*
Zkopíruje hodnotu ⟨symb⟩ do ⟨var⟩. 
provede zkopírování hod-
noty proměnné var v globálním rámci do proměnné par v lokálním rámci
*/
void gen_move(char *var, char *symb);

/*
Vytvoří nový dočasný rámec a zahodí případný obsah původního dočasného rámce.
*/
void gen_createframe();

/*
Přesune TF na zásobník rámců.
TF bude po provedení instrukce nedefinován a je třeba jej před dalším
použitím vytvořit pomocí CREATEFRAME. 
*/
void gen_pushframe();


/*
Přesune vrcholový rámec LF ze zásobníku rámců do TF.
*/
void gen_popframe();

/*
Definuje proměnnou v určeném rámci dle ⟨var⟩
*/
void gen_defvar(char *var);

/*
Uloží inkrementovanou aktuální pozici z interního čítače instrukcí do zásobníku volání a pro-
vede skok na zadané návěští 
*/
void gen_call(char *label);

/*
Vyjme pozici ze zásobníku volání a skočí na tuto pozici nastavením interního čítače instrukcí
(úklid lokálních rámců musí zajistit jiné instrukce).
*/
void gen_return();

/*
Uloží hodnotu ⟨symb⟩ na datový zásobník.
*/
//TODO when i find out where data type is i will repair this function
void gen_pushs(char *symb, data_type type);

/*
Vyjme hodnotu ze zásobníku a uloží ji do proměnné ⟨var⟩.
*/
void gen_pops(char *var);

/*
Smaže celý obsah datového zásobníku
*/
void gen_clears();

/*
Sečte ⟨symb1⟩ a ⟨symb2⟩ a výslednou hodnotu téhož typu uloží do proměnné ⟨var⟩.
*/
void gen_add(char *var, char *symb1, char *symb2);

/*
Odečte ⟨symb2⟩ od ⟨symb1⟩ a výslednou
hodnotu téhož typu uloží do proměnné ⟨var⟩.
*/
void gen_sub(char *var, char *symb1, char *symb2);

/*
Vynásobí ⟨symb1⟩ a ⟨symb2⟩ a výslednou
hodnotu téhož typu uloží do proměnné ⟨var⟩.
*/
void gen_mul(char *var, char *symb1, char *symb2);

/*
Podělí hodnotu ze ⟨symb1⟩ druhou hodnotou ze ⟨symb2⟩ (oba musí být typu float) a výsledek
přiřadí do proměnné ⟨var⟩ (též typu float). 
*/
void gen_div(char *var, char *symb1, char *symb2);

/*
Celočíselně podělí hodnotu ze ⟨symb1⟩ druhou hodnotou ze ⟨symb2⟩ (musí být oba typu int)
a výsledek (zaokrouhlený k nule) přiřadí do proměnné ⟨var⟩ typu int. 
*/
void gen_idiv(char *var, char *symb1, char *symb2);

/*

Porovná hodnoty ⟨symb1⟩ a ⟨symb2⟩ a výslednou hodnotu typu bool uloží do proměnné ⟨var⟩.
*/
//LT
void gen_LT(char *var, char *symb1, char *symb2);
//GT
void gen_GT(char *var, char *symb1, char *symb2);
//EQ
void gen_EQ(char *var, char *symb1, char *symb2);

/*
Základní booleovské operátory
Aplikuje konjunkci (logické A)/disjunkci (logické NEBO) na operandy typu bool ⟨symb1⟩
a ⟨symb2⟩ nebo negaci na ⟨symb1⟩ (NOT má pouze 2 operandy) a výsledek typu bool zapíše
do ⟨var⟩.
*/
//AND
void gen_AND(char *var, char *symb1, char *symb2);
//OR
void gen_OR(char *var, char *symb1, char *symb2);
//NOT
void gen_NOT(char *var, char *symb1);

/*
Převede celočíselnou hodnotu ⟨symb⟩ na desetinné číslo a uloží jej do ⟨var.⟩
*/
void gen_int2float(char *var, char *symb);

/*
Převede desetinnou hodnotu ⟨symb⟩ na celočíselnou oseknutím desetinné části a uloží ji do
⟨var.⟩
*/
void gen_float2int(char *var, char *symb);


/*
Číselná hodnota ⟨symb⟩ je dle ASCII převedena na znak, který tvoří jednoznakový řetězec
přiřazený do ⟨var⟩. Je-li ⟨symb⟩ mimo interval ⟨0; 127⟩, dojde k chybě 58.
*/
void gen_int2char(char *var, char *symb);

/*
Do ⟨var⟩ uloží ordinální hodnotu znaku (dle ASCII) v řetězci ⟨symb1⟩ na pozici ⟨symb2⟩ (in-
dexováno od nuly). Indexace mimo daný řetězec vede na chybu 58.
*/
void gen_str2int(char *var, char *symb1, char *symb2);

/*
Převede desetinnou hodnotu ⟨symb⟩ na řetězec a uloží jej do ⟨var.⟩ Hodnota je převedena v sou-
ladu s chováním vestavěné funkce toStr jazyka IFJ26.
*/
void gen_float2str(char *var, char *symb);

/*
Převede celočíselnou hodnotu ⟨symb⟩ na řetězec a uloží jej do ⟨var.⟩ Hodnota je převedena
v souladu s chováním vestavěné funkce toStr jazyka IFJ26
*/
void gen_int2str(char *var, char *symb);

/*
Načte jednu hodnotu dle zadaného typu ⟨type⟩ ∈ {int, float, string, bool} a uloží tuto hod-
notu do proměnné ⟨var⟩. 
Formát hodnot je kompatibilní s chováním vestavěných
funkcí readLine, readInt, readDouble (a příp. readBoolean) jazyka IFJ26
*/
void gen_read(char *var, char *type);

/*
Vypíše hodnotu ⟨symb⟩ na standardní výstup. Hodnota typu float je vypsána pomocí formá-
tovacího řetězce %a (pozor, rozdíl proti instrukci FLOAT2STR), hodnota typu int je vypsána
pomocí řetězce %d.
*/
void gen_write(char *symb);

/*
Do proměnné ⟨var⟩ uloží řetězec vzniklý konkatenací dvou řetězcových operandů ⟨symb1⟩
a ⟨symb2⟩
*/
void gen_concat(char *var, char *symb1, char *symb2);

/*
Zjistí délku řetězce v ⟨symb⟩ a délka je uložena jako celé číslo do ⟨var.⟩
*/
void gen_strlen(char *var, char *symb);

/*
Do ⟨var⟩ uloží řetězec z jednoho znaku v řetězci ⟨symb1⟩ na pozici ⟨symb2⟩ (indexováno celým
číslem od nuly).
*/
void gen_getchar(char *var, char *symb1, char *symb2);

/*
Zmodifikuje znak řetězce uloženého v proměnné ⟨var⟩ na pozici ⟨symb1⟩ (indexováno celo-
číselně od nuly) na znak v řetězci ⟨symb2⟩ (první znak, pokud obsahuje ⟨symb2⟩ více znaků).
Výsledný řetězec je opět uložen do ⟨var.⟩
*/
void gen_setchar(char *var, char *symb1, char *symb2);

/*
Vytvoří pole o velikosti ⟨symb𝑠⟩ a uloží referenci na něj do proměnné ⟨var⟩.
*/
void gen_newarray(char *var, char *symbs);

/*
Přečte prvek z pole odkazovaného z proměnné ⟨var𝑎⟩ na indexu ⟨symb𝑖𝑑𝑥⟩ do proměnné ⟨var𝑡⟩
*/
void gen_arrayget(char *var, char *var_a, char *symb_index);

/*
Do pole odkazovaného z proměnné ⟨var𝑎⟩ uloží na index ⟨symb𝑖𝑑𝑥⟩ prvek ⟨symb𝑣𝑎𝑙⟩
*/
void gen_arrayset(char *var_a, char *symb_index, char *symb_val);

/*
Dynamicky zjistí typ symbolu ⟨symb⟩ a do ⟨var⟩ zapíše řetězec značící tento typ (int, bool,
float, string, nil, array)
*/
void gen_type(char *var, char *symb);

/*
Určí, jestli je číslo v symbolu ⟨symb⟩ celé (tj. typu int nebo typu float s nulovou desetinnou
částí), a výsledek typu bool zapíše do ⟨var⟩
*/
void gen_isint(char *var, char *symb);

/*Flow control instructions*/
/*
Speciální instrukce označující pomocí návěští ⟨label-literal⟩ důležitou pozici v kódu jako po-
tenciální cíl libovolné skokové instrukce.
*/
void gen_label(char *label);

/*
Provede nepodmíněný skok na zadané návěští ⟨label⟩.
*/
void gen_jump(char *label);

/*
JUMPIFEQ - skok na zadané návěští, pokud jsou dva symboly rovny
JUMPIFNEQ - skok na zadané návěští, pokud jsou dva symboly nerovny
*/
void gen_jumpifeq(char *label, char *symb1, char *symb2);
void gen_jumpifneq(char *label, char *symb1, char *symb2);

/*
Ukončí vykonávání programu a ukončí interpret s návratovým kódem ⟨symb⟩, kde ⟨symb⟩ je
celé číslo v intervalu 0 až 49 (včetně). Nevalidní celočíselná hodnota ⟨symb⟩ vede na chybu
57.
*/
void gen_exit(char *symb);

/*
Na standardní chybový výstup (stderr) vypíše stav interpretu v danou chvíli (tj. během vykoná-
vání této instrukce). Stav se mimo jiné skládá z pozice v kódu, výpisu globálního, aktuálního
lokálního a dočasného rámce a počtu již vykonaných instrukcí.
*/
void gen_break();

/*
Vypíše diagnostický zápis dané hodnoty ⟨symb⟩ na standardní chybový výstup (stderr). Umož-
ňuje vypsat i pole a neinicializované proměnné. Výpisy touto instrukcí bude možné vypnout
pomocí volby interpretu (viz nápověda interpretu)
*/
void gen_dprint(char *symb);

/*
 * Stack versions of arithmetic, relational, boolean, conversion, type and
 * conditional jump instructions.
 */
void gen_adds(void);
void gen_subs(void);
void gen_muls(void);
void gen_divs(void);
void gen_idivs(void);

void gen_LTs(void);
void gen_GTs(void);
void gen_EQs(void);

void gen_ANDs(void);
void gen_ORs(void);
void gen_NOTs(void);

void gen_int2floats(void);
void gen_float2ints(void);
void gen_int2chars(void);
void gen_stri2ints(void);
void gen_float2strs(void);
void gen_int2strs(void);

void gen_types(void);
void gen_isints(void);

void gen_jumpifeqs(char *label);
void gen_jumpifneqs(char *label);

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