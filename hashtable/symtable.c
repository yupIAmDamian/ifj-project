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
    ht_clear(&(st->scopes[st->top]));
    st->top--;
}

bool symtable_insert_var(Symtable *st, const char *name, DataType type, bool is_init) {
    if (st->top < 0 || !name) return false;
    //need a func to check if variable is not redefined

    Symbol *sym = malloc(sizeof(Symbol));
    if (!sym) return false;
    sym->name = strdup(name);
    sym->s_type = SYMBOL_VAR;
    sym->d_type = type;
    sym->is_initialized = is_init;
    sym->params_amount=0;
    sym->params=NULL;

    ht_item_t *inserted_item = ht_insert(st->scopes[st->top], name, (const void*) sym);
    return (inserted_item != NULL);
}

Symbol* symtable_search_current_scope(Symtable *st, const char *name) {
    if (st->top < 0 || !name) return NULL;
    ht_item_t *item = ht_search(st->scopes[st->top], name);
    
    // void* data back to Symbol*
    if (item != NULL && item->state == OCCUPIED) {
        return (Symbol*) item->data; 
    }
    return NULL;
}

Symbol* symtable_search(Symtable *st, const char *name) {
    if (!name) return NULL;
    for (int i = st->top; i >= 0; i--) {
        ht_item_t *item = ht_search(st->scopes[i], name);
        if (item != NULL && item->state == OCCUPIED) {
            return (Symbol*) item->data;
        }
    }
    return NULL;
}

bool symtable_insert_func(Symtable *st, const char *name, DataType return_type, Param *params, int param_count) {
    if (st->top < 0 || !name) return false;
    
    ht_item_t *existing = ht_search(st->scopes[0], name);
    if (existing != NULL && existing->state == OCCUPIED) {
        return false;
    }
    Symbol *sym = malloc(sizeof(Symbol));
    if (!sym) return false;

    
    sym->name = strdup(name);
    sym->s_type = SYMBOL_FUNC;
    sym->d_type = return_type;
    sym->is_initialized = true;
    sym->params_amount = param_count;
    
    if (param_count > 0 && params != NULL) {
        sym->params = malloc(sizeof(Param) * param_count);
        if (!sym->params) {
            free(sym->name);
            free(sym);
            return false;
        }
        
        for (int i = 0; i < param_count; i++) {
            sym->params[i].name = strdup(params[i].name);
            sym->params[i].d_type = params[i].d_type;
        }
    } else {
        sym->params = NULL;
    }
    
    ht_item_t *inserted_item = ht_insert(st->scopes[0], name, (const void*) sym);
    return (inserted_item != NULL);
}