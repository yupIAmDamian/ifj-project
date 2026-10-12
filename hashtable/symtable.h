#include <stdbool.h>
#include <stddef.h>
#include "hashtable.h"

#define MAX_SCOPES 100

typedef enum{
    TYPE_UNKNOWN,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_VOID,
    TYPE_STRING,
    TYPE_BOOL
} DataType;

typedef enum{
    SYMBOL_VAR,
    SYMBOL_FUNC
} SymbolType;

typedef struct Param{
    char *name;
    DataType d_type;
} Param;

typedef struct Symbol{
    char *name;
    SymbolType s_type;
    DataType d_type;
    bool is_initialized;
    int params_amount;
    Param *params;
} Symbol;

typedef struct {
    hashtable_t *scopes[MAX_SCOPES]; 
    int top;                      
} Symtable;


Symtable* symtable_init(void);
void symtable_dispose(Symtable *st);

void scope_push(Symtable *st);
void scope_pop(Symtable *st);

bool symtable_insert_var(Symtable *st, const char *name, DataType type, bool is_init);
bool symtable_insert_func(Symtable *st, const char *name, DataType return_type, Param *params, int param_count);

Symbol* symtable_search(Symtable *st, const char *name);
Symbol* symtable_search_current_scope(Symtable *st, const char *name);