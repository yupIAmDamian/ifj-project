// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)

#ifndef SYMTABLE_TESTS_H
#define SYMTABLE_TESTS_H

#include "../../hashtable/symtable.h"
#include "../../helper_func.h"

void test_devider(char *test_name);
void symbol_test(char *name, SymbolType s_type, DataType d_type, bool is_init, bool is_const);
void symtable_init_test();
void symbol_insertion_test();
void search_current_scope_test();
void search_in_all_scopes_test();
void function_insertion_and_search_test();
void double_function_insertion_test();
void scope_push_and_pop_test();
void capacity_and_rehashing_test();
int main();

#endif 