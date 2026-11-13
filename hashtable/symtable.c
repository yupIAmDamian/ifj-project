// Compiler of IFJ 2026 imperative language
// Author: Daniel Jurkovič (xjurkod00)

#include <stdlib.h>
#include "symtable.h"
#include "../helper_func.h"

//TODO function for checking if name is assigned or no
Symbol* symbol_create(const char *name, SymbolType s_type, DataType d_type, bool is_init) {
    if (!name) return NULL;

    Symbol *sym = malloc(sizeof(Symbol));
    if (!sym) return NULL;

    sym->name = strdup(name);
    if (!sym->name) {
        free(sym);
        return NULL;
    }

    sym->s_type = s_type;
    sym->d_type = d_type;
    sym->is_initialized = is_init;
    sym->params_amount = 0;
    sym->params = NULL;

    return sym;
}

void symbol_free(Symbol *sym) {
    if (!sym) return;

    if (sym->name) {
        free(sym->name);
    }
    if (sym->params) {
        for (int i = 0; i < sym->params_amount; i++) {
            if (sym->params[i].name) {
                free(sym->params[i].name);
            }
        }
        free(sym->params);
    }
    free(sym);
}

Symtable* symtable_init(void) {
    Symtable *st = malloc(sizeof(Symtable));
    if (!st) return NULL;
    st->top = -1;
    st->block_counter=0;
    scope_push(st);
    return st;
}

void symtable_dispose(Symtable *st) {
    if (!st) return;
    while (st->top >= 0) {
        scope_pop(st);
    }
    free(st);
}

void scope_push(Symtable *st) {
    if (st->top >= MAX_SCOPES - 1) return;
    st->top++;
    st->block_counter++;
    st->scopes[st->top] = ht_create(100);
}


//damian pls check if this works correctly
void scope_pop(Symtable *st) {
    if (st->top < 0) return;

    hashtable_t *table = st->scopes[st->top];
    if (table != NULL && table->items != NULL) {
            for (int i = 0; i < table->size; i++) {
                if (table->items[i].state == OCCUPIED && table->items[i].data != NULL) {
                    symbol_free((Symbol*) table->items[i].data);
                    table->items[i].data = NULL; //this fixes double free I guess
                }
            }
        ht_clear(&(st->scopes[st->top]));
    }
    st->top--;
}

bool symtable_insert_var(Symtable *st, char *name, DataType type, bool is_init) {
    if (st->top < 0 || !name) return false;

    if (symtable_search_current_scope(st, name) != NULL) {
        return false;
    }

    Symbol *sym = symbol_create(name, SYMBOL_VAR, type, is_init);
    if (!sym) return false;

    char mangled_buf[256];
    if (st->top == 0) {
        snprintf(mangled_buf, sizeof(mangled_buf), "GF@%s", name);
    } 
    else {
        snprintf(mangled_buf, sizeof(mangled_buf), "LF@%s$%d", name, st->block_counter);
    }

    sym->mangled_name = strdup(mangled_buf);
    ht_item_t *inserted_item = ht_insert(st->scopes[st->top], name, (void*) sym);
    if (!inserted_item) {
        if (sym->mangled_name) free(sym->mangled_name);
        symbol_free(sym);
        return false;
    }

    return true;
}

Symbol* symtable_search_current_scope(Symtable *st, char *name) {
    if (st->top < 0 || !name) return NULL;
    ht_item_t *item = ht_search(st->scopes[st->top], name);
    
    if (item != NULL && item->state == OCCUPIED) {
        return (Symbol*) item->data; 
    }
    return NULL;
}

Symbol* symtable_search(Symtable *st, char *name) {
    if (!name) return NULL;
    for (int i = st->top; i >= 0; i--) {
        ht_item_t *item = ht_search(st->scopes[i], name);
        if (item != NULL && item->state == OCCUPIED) {
            return (Symbol*) item->data;
        }
    }
    return NULL;
}

bool symtable_insert_func(Symtable *st, char *name, DataType return_type, Param *params, int param_count) {
    if (st->top < 0 || !name) return false;
    
    ht_item_t *existing = ht_search(st->scopes[0], name);
    if (existing != NULL && existing->state == OCCUPIED) {
        return false;
    }

    Symbol *sym = symbol_create(name, SYMBOL_FUNC, return_type, true);
    if (!sym) return false;

    if (param_count > 0 && params != NULL) {
        sym->params = malloc(sizeof(Param) * param_count);
        if (!sym->params) {
            symbol_free(sym);
            return false;
        }
        sym->params_amount = param_count;

        for (int i = 0; i < param_count; i++) {
            sym->params[i].name = str_duplicate(params[i].name);
            if (!sym->params[i].name) {
                free(sym);
                return false;
            }
            sym->params[i].d_type = params[i].d_type;
            if (!sym->params[i].name) {
                symbol_free(sym);
                return false;
            }
        }
    }

    ht_item_t *inserted_item = ht_insert(st->scopes[0], name, (const void*) sym);
    if (!inserted_item) {
        symbol_free(sym);
        return false;
    }
    return true;
}