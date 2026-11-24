// Compiler of IFJ 2026 imperative language
// Author: Daniel Jurkovič (xjurkod00)

#include <stdlib.h>
#include "symtable.h"
#include "../helper_func.h"
#include <string.h>
#include <stdio.h>

/**
 * @brief Allocates and initializes a new Symbol structure.
 * @param name Identifier name of the symbol (duplicated internally).
 * @param s_type Kind of symbol (variable or function).
 * @param d_type Data type of the variable or return type of the function.
 * @param is_init Indicates whether the symbol has been initialized.
 * @param is_const Indicates whether the symbol is immutable (val or parameter).
 * @return Pointer to the newly allocated Symbol, or NULL on invalid input or allocation failure.
 */
//TODO function for checking if name is assigned or no
Symbol* symbol_create(char *name, SymbolType s_type, DataType d_type, bool is_init, bool is_const) {
    if (!name) return NULL;

    Symbol *sym = malloc(sizeof(Symbol));
    if (!sym) return NULL;

    sym->name = str_duplicate(name);
    if (!sym->name) {
        free(sym);
        return NULL;
    }

    sym->mangled_name = NULL;
    sym->s_type = s_type;
    sym->d_type = d_type;
    sym->is_initialized = is_init;
    sym->is_const = is_const;
    sym->params_amount = 0;
    sym->params = NULL;
    sym->is_const = false;

    return sym;
}

/**
 * @brief Frees all dynamically allocated memory associated with a Symbol structure.
 * @param sym Pointer to the Symbol to free.
 */
void symbol_free(Symbol *sym) {
    if (!sym) return;

    if (sym->name) {
        free(sym->name);
    }
    if (sym->mangled_name) {
        free(sym->mangled_name);
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

/**
 * @brief Allocates and initializes a new symbol table with an active global scope.
 * @return Pointer to the initialized Symtable, or NULL on allocation failure.
 */
Symtable* symtable_init() {
    Symtable *st = malloc(sizeof(Symtable));
    if (!st) return NULL;
    st->top = -1;
    st->block_counter=0;
    st->in_main=false;
    st->current_func =NULL;
    scope_push(st);
    return st;
}

/**
 * @brief Disposes of the entire symbol table, freeing all scopes, stored symbols, and the table itself.
 * @param st Pointer to the Symtable to dispose of.
 */
void symtable_dispose(Symtable *st) {
    if (!st) return;
    while (st->top >= 0) {
        scope_pop(st);
    }
    free(st);
}

/**
 * @brief Closes the current innermost lexical scope, freeing all symbols within it and popping its hash table.
 * @param st Pointer to the Symtable.
 */
void scope_pop(Symtable *st) {
    if (st->top < 0) return;

    hashtable_t *table = st->scopes[st->top];
    if (table != NULL && table->items != NULL) {
        //free all symbols - dispose of data from hashtable items
        for (int i = 0; i < table->size; i++) {
            if ((table->items[i].state == OCCUPIED || table->items[i].state == DELETED) && table->items[i].data != NULL) {
                symbol_free((Symbol*) table->items[i].data);
            }
        }
        
        //free the hashtable itself
        ht_clear(&(st->scopes[st->top]));
    }
    st->top--;
}

/**
 * @brief Opens a new lexical scope by pushing a new hash table onto the scope stack.
 * @param st Pointer to the Symtable.
 */
void scope_push(Symtable *st) {
    if (st->top >= MAX_SCOPES - 1) return;
    st->top++;
    st->block_counter++;
    st->scopes[st->top] = ht_create(100);
}

/**
 * @brief Inserts a new variable symbol into the current (topmost) lexical scope and generates its mangled name.
 * @param st Pointer to the Symtable.
 * @param name Original identifier name of the variable.
 * @param type Data type of the variable.
 * @param is_init Indicates whether the variable is initialized upon declaration.
 * @param is_const Indicates whether the variable is constant (val) or mutable (var).
 * @return true if the variable was inserted successfully, false on redefinition or internal error.
 */
//TODO
//One false for 3 different situations. symtable_insert_var returns false for redefinition, for a full table (100 items per scope), and for allocation failure. The semantic analyzer needs to distinguish redefinition, which is a program error, from an internal error.
bool symtable_insert_var(Symtable *st, char *name, DataType type, bool is_init, bool is_const) {
    if (st->top < 0 || !name) return false;

    if (symtable_search_current_scope(st, name) != NULL) {
        return false;
    }

    Symbol *sym = symbol_create(name, SYMBOL_VAR, type, is_init, is_const);
    if (!sym) return false;

    char mangled_buf[512];
    const char *fn = (st->current_func != NULL) ? st->current_func : "global";

    if (st->in_main) {
        snprintf(mangled_buf, sizeof(mangled_buf), "GF@%s$%s$%d", fn, name, st->block_counter);
    } else {
        snprintf(mangled_buf, sizeof(mangled_buf), "LF@%s$%s$%d", fn, name, st->block_counter);
}

    sym->mangled_name = str_duplicate(mangled_buf);
    sym->is_const = is_const;
    ht_item_t *inserted_item = ht_insert(st->scopes[st->top], name, (void*) sym);
    if (!inserted_item) {
        symbol_free(sym);
        return false;
    }

    return true;
}

/**
 * @brief Searches for a symbol only within the current (topmost) lexical scope.
 * @param st Pointer to the Symtable.
 * @param name Identifier name to search for.
 * @return Pointer to the matching Symbol in the current scope, or NULL if not found.
 */
Symbol* symtable_search_current_scope(Symtable *st, char *name) {
    if (st->top < 0 || !name) return NULL;
    ht_item_t *item = ht_search(st->scopes[st->top], name);
    
    if (item != NULL && item->state == OCCUPIED) {
        return (Symbol*) item->data; 
    }
    return NULL;
}

/**
 * @brief Searches for a symbol across all active lexical scopes from innermost to global.
 * @param st Pointer to the Symtable.
 * @param name Identifier name to search for.
 * @return Pointer to the closest matching Symbol, or NULL if not found.
 */
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

/**
 * @brief Inserts a new function symbol along with a deep copy of its parameters into the global scope.
 * @param st Pointer to the Symtable.
 * @param name Identifier name of the function.
 * @param return_type Return data type of the function.
 * @param params Array of formal parameter descriptors (can be NULL if param_count is 0).
 * @param param_count Number of formal parameters.
 * @return true if the function was inserted successfully, false on redefinition or allocation failure.
 */
bool symtable_insert_func(Symtable *st, char *name, DataType return_type, Param *params, int param_count) {
    if (st->top < 0 || !name) return false;
    
    ht_item_t *existing = ht_search(st->scopes[0], name);
    if (existing != NULL && existing->state == OCCUPIED) {
        return false;
    }

    Symbol *sym = symbol_create(name, SYMBOL_FUNC, return_type, true, false);
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
                sym->params_amount=i;
                symbol_free(sym);
                return false;
            }
            sym->params[i].d_type = params[i].d_type;
        }
    }

    ht_item_t *inserted_item = ht_insert(st->scopes[0], name, (void*) sym);
    if (!inserted_item) {
        symbol_free(sym);
        return false;
    }
    return true;
}