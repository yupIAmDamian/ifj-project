// Compiler of IFJ26 imperative language
// Authors: Damian Frantisek Girasek (xgirasd00), Daniel Jurkovič (xjurkod00),
//          Dominik Madaj (xmadajd00), Michal Kopček (xkopcem00)

#include "scanner.h"
#include "error.h"
#include "ast.h"
#include "symtable.h"
#include "expression.h"
#include "parser.h"
#include "codegen.h"
// pridat vsetky potrebne .h subory


// symtable
static Symtable *st = NULL;

// function for freeing the symtable, registered in atexit
static void symtable_cleanup(void) {
    symtable_dispose(st);
    st = NULL;
}


int main() {
    // registers all cleanup functions
    error_register_cleanup(symtable_cleanup); // danko
    error_register_cleanup(ast_cleanup); // misko
    // error_register_cleanup(expr_stack_cleanup); // miskov stack free wrapnuty, odkomentuj ked bude
    error_register_cleanup(scanner_free); // dominik

    scanner_init();

    // HLAVNA CAST PROGRAMU
    // neviem co a ako sa ma zavolat tu
    // treba doplnit


    return 0; // all cleanup functions are called
}