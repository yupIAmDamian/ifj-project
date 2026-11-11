#include <stdio.h>
#include "hashtable_func.h"
#include "../../hashtable/symtable.h"

int symtable_tests() {
    Symtable* st = symtable_init();
    if (st == NULL) {
        printf("Failed to initialize symbol table\n");
        return 1;
    }

    scope_push(st);

    if (!symtable_insert_var(st, "var1", TYPE_INT, true)) {
        printf("Failed to insert var1\n");
        return 1;
    }

    Symbol* sym = symtable_search(st, "var1");
    if (sym != NULL) {
        printf("Found var1: type=%d, initialized=%d\n", sym->d_type, sym->is_initialized);
    } else {
        printf("var1 not found\n");
    }

    scope_pop(st);

    sym = symtable_search(st, "var1");
    if (sym != NULL) {
        printf("Found var1 after popping scope: type=%d, initialized=%d\n", sym->d_type, sym->is_initialized);
    } else {
        printf("var1 not found after popping scope\n");
    }

    symtable_dispose(st);

    return 0;
}