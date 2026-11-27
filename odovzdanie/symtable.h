// Compiler of IFJ 2026 imperative language
// Author: Daniel Jurkovič (xjurkod00)

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "error.h"
#include "hashtable.h"
#include "helper_func.h"
#include <stdbool.h>
#include <stddef.h>

#define MAX_SCOPES 100

/**
 * @brief Represents supported data types in the IFJ26 language.
 */
typedef enum {
  TYPE_UNKNOWN,
  TYPE_INT,
  TYPE_FLOAT,
  TYPE_VOID,
  TYPE_STRING,
  TYPE_BOOL
} DataType;

/**
 * @brief Distinguishes whether a symbol represents a variable or a function.
 */
typedef enum { SYMBOL_VAR, SYMBOL_FUNC } SymbolType;

/**
 * @brief Stores metadata for a single formal parameter of a function.
 */
typedef struct Param {
  char *name;      // Parameter identifier name
  DataType d_type; // Parameter data type
} Param;

/**
 * @brief Represents a symbol table entry storing semantic and code-generation
 * attributes of a variable or function.
 */
typedef struct Symbol {
  char *name;         // Original identifier name
  char *mangled_name; // Unique mangled name with frame prefix (e.g., "LF@x$1")
  SymbolType s_type;  // Kind of symbol (variable or function)
  DataType d_type; // Data type of the variable or return type of the function
  bool is_initialized; // Indicates whether the symbol has been initialized
  bool is_const; // Indicates whether the symbol is immutable (val or parameter)
  int params_amount; // Number of formal parameters (for functions)
  Param *params;     // Dynamically allocated array of function parameters
} Symbol;

/**
 * @brief Represents the symbol table implemented as a stack of hash tables for
 * lexical scope management.
 */
typedef struct {
  hashtable_t
      *scopes[MAX_SCOPES]; // Stack of hash tables representing nested scopes
  int top;                 // Index of the current innermost scope (-1 if empty)
  int block_counter; // Monotonic counter for unique variable name mangling
  bool in_main;      // True when analyzing the '@main' entry function (uses GF@
                     // frame)
  char *current_func; // Name of analysed func
} Symtable;

/**
 * @brief Allocates and initializes a new Symbol structure.
 * @param name Identifier name of the symbol (duplicated internally).
 * @param s_type Kind of symbol (variable or function).
 * @param d_type Data type of the variable or return type of the function.
 * @param is_init Indicates whether the symbol has been initialized.
 * @param is_const Indicates whether the symbol is immutable (val or parameter).
 * @return Pointer to the newly allocated Symbol, or NULL on invalid input or
 * allocation failure.
 */
Symbol *symbol_create(char *name, SymbolType s_type, DataType d_type,
                      bool is_init, bool is_const);

/**
 * @brief Frees all dynamically allocated memory associated with a Symbol
 * structure.
 * @param sym Pointer to the Symbol to free.
 */
void symbol_free(Symbol *sym);

/**
 * @brief Allocates and initializes a new symbol table with an active global
 * scope.
 * @return Pointer to the initialized Symtable, or NULL on allocation failure.
 */
Symtable *symtable_init();

/**
 * @brief Disposes of the entire symbol table, freeing all scopes, stored
 * symbols, and the table itself.
 * @param st Pointer to the Symtable to dispose of.
 */
void symtable_dispose(Symtable *st);

/**
 * @brief Opens a new lexical scope by pushing a new hash table onto the scope
 * stack.
 * @param st Pointer to the Symtable.
 */
void scope_push(Symtable *st);

/**
 * @brief Closes the current innermost lexical scope, freeing all symbols within
 * it and popping its hash table.
 * @param st Pointer to the Symtable.
 */
void scope_pop(Symtable *st);

/**
 * @brief Inserts a new variable symbol into the current (topmost) lexical scope
 * and generates its mangled name.
 * @param st Pointer to the Symtable.
 * @param name Original identifier name of the variable.
 * @param type Data type of the variable.
 * @param is_init Indicates whether the variable is initialized upon
 * declaration.
 * @param is_const Indicates whether the variable is constant (val) or mutable
 * (var).
 * @return true if the variable was inserted successfully, false on redefinition
 * or internal error.
 */
bool symtable_insert_var(Symtable *st, char *name, DataType type, bool is_init,
                         bool is_const);

/**
 * @brief Inserts a new function symbol along with a deep copy of its parameters
 * into the global scope.
 * @param st Pointer to the Symtable.
 * @param name Identifier name of the function.
 * @param return_type Return data type of the function.
 * @param params Array of formal parameter descriptors (can be NULL if
 * param_count is 0).
 * @param param_count Number of formal parameters.
 * @return true if the function was inserted successfully, false on redefinition
 * or allocation failure.
 */
bool symtable_insert_func(Symtable *st, char *name, DataType return_type,
                          Param *params, int param_count);

/**
 * @brief Searches for a symbol across all active lexical scopes from innermost
 * to global.
 * @param st Pointer to the Symtable.
 * @param name Identifier name to search for.
 * @return Pointer to the closest matching Symbol, or NULL if not found.
 */
Symbol *symtable_search(Symtable *st, char *name);

/**
 * @brief Searches for a symbol only within the current (topmost) lexical scope.
 * @param st Pointer to the Symtable.
 * @param name Identifier name to search for.
 * @return Pointer to the matching Symbol in the current scope, or NULL if not
 * found.
 */
Symbol *symtable_search_current_scope(Symtable *st, char *name);

#endif