#include <stdlib.h>
#include <string.h>
#include "symtable.h"

Symtable* symtable_init(void){
    Symtable *st = malloc(sizeof(Symtable));
    if (!st) return NULL;
    st->top=-1;
    scope_push(st);
    return st;
}
void symtable_dispose(Symtable *st){
    if (!st) return;
    while (st->top >=0){
        scope_pop(st);
    }
    free(st);
}

void scope_push(Symtable *st){
    if (st->top>=MAX_SCOPES) return;
    st->top++;
    st->scopes[st->top] =ht_create(100);
}

void scope_pop(Symtable *st){
    if (st->top <0) return;
    st->scopes[st->top]=ht_clear(&(st->scopes[st->top]));
}