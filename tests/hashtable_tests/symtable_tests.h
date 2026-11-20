// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)
#ifndef SYMTABLE_TESTS_H
#define SYMTABLE_TESTS_H

#include "../../hashtable/symtable.h"
#include "../../helper_func.h"

/**
 * @brief Prints a formatted header banner to visually separate test cases in console output.
 * @param test_name String naming the test being executed.
 */
void test_devider(char *test_name);

/**
 * @brief Tests symbol creation, verifies all assigned properties, and frees the allocated symbol.
 * @param name Identifier name of the symbol.
 * @param s_type Type of symbol.
 * @param d_type Data type associated with the symbol.
 * @param is_init Flag indicating whether the symbol is initialized.
 * @param is_const Flag indicating whether the symbol is a constant.
 */
void symbol_test(char *name, SymbolType s_type, DataType d_type, bool is_init, bool is_const);

/**
 * @brief Tests symbol table initialization and verifies default stack top, block counter, and main scope flags.
 */
void symtable_init_test();

/**
 * @brief Tests variable insertion into the symbol table, duplicate rejection, and retrieval in the current scope.
 */
void symbol_insertion_test();

/**
 * @brief Tests searching for an inserted variable symbol strictly within the active scope.
 */
void search_current_scope_test();

/**
 * @brief Tests hierarchical symbol lookup across both outer and nested scopes.
 */
void search_in_all_scopes_test();

/**
 * @brief Tests function symbol insertion, parameter count verification, and frame-based name mangling across scopes.
 */
void function_insertion_and_search_test();

/**
 * @brief Tests NULL argument handling, duplicate function rejection, and local variable shadowing of function names.
 */
void double_function_insertion_test();

/**
 * @brief Tests pushing and popping scopes, verifying inner scope cleanup and variable shadowing behavior.
 */
void scope_push_and_pop_test();

 /**
 * @brief Tests symbol table capacity limits, bulk insertion/retrieval, and scope stack overflow protection.
 */
void capacity_and_rehashing_test();

/**
 * @brief Entry point that runs the complete suite of unit tests for the symbol table implementation.
 * @return 0 upon completion of all test suites.
 */
int main();

#endif 