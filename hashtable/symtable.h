#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdbool.h>
#include <stddef.h>
#include "hashtable.h"
#include "../helper_func.h"

#define MAX_SCOPES 100

typedef enum {
    TYPE_UNKNOWN,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_VOID,
    TYPE_STRING,
    TYPE_BOOL
} DataType;

typedef enum {
    SYMBOL_VAR,
    SYMBOL_FUNC
} SymbolType;

typedef struct Param {
    char *name;
    DataType d_type;
} Param;

typedef struct Symbol {
    char *name;
    char *mangled_name;
    SymbolType s_type;
    DataType d_type;
    bool is_initialized;
    bool is_const;
    int params_amount;
    Param *params;
} Symbol;

typedef struct {
    hashtable_t *scopes[MAX_SCOPES]; 
    int top;
    int block_counter; //counting blocks for mangling
    bool in_main;                   
} Symtable;

Symbol* symbol_create(char *name, SymbolType s_type, DataType d_type, bool is_init);
void symbol_free(Symbol *sym);

Symtable* symtable_init(void);
void symtable_dispose(Symtable *st);

void scope_push(Symtable *st);
void scope_pop(Symtable *st);

bool symtable_insert_var(Symtable *st, char *name, DataType type, bool is_init, bool is_const);
bool symtable_insert_func(Symtable *st, char *name, DataType return_type, Param *params, int param_count);

Symbol* symtable_search(Symtable *st, char *name);
Symbol* symtable_search_current_scope(Symtable *st, char *name);

#endif