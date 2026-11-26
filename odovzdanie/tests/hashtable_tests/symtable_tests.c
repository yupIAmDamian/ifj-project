// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)
#include "symtable_tests.h"
#include <stdio.h>

void test_devider(char *test_name) {
  printf("\n====================\n");
  printf("Running test: %s\n", test_name);
  printf("====================\n");
}

void symbol_test(char *name, SymbolType s_type, DataType d_type, bool is_init,
                 bool is_const) {
  Symbol *sym = symbol_create(name, s_type, d_type, is_init, is_const);
  if (!sym) {
    printf("[FAIL] Failed to create symbol: %s\n", name);
    return;
  }

  if (!matching_string(sym->name, name) || sym->s_type != s_type ||
      sym->d_type != d_type || sym->is_initialized != is_init ||
      sym->is_const != is_const) {
    printf("[FAIL] Symbol properties do not match for: %s\n", name);
  } else {
    printf("[OK] Symbol test passed for: %s\n", name);
  }

  symbol_free(sym);
}

void symtable_init_test() {
  Symtable *st = symtable_init();
  if (!st) {
    printf("[FAIL] Failed to initialize symbol table\n");
    return;
  }

  if (st->top != 0 || st->block_counter != 1 || st->in_main != false) {
    printf(
        "[FAIL] Symbol table properties do not match after initialization\n");
    printf("top: %d, block_counter: %d, in_main: %d\n", st->top,
           st->block_counter, st->in_main);
  } else {
    printf("[OK] Symbol table initialization test passed\n");
  }

  symtable_dispose(st);
}

void symbol_insertion_test() {
  Symtable *st = symtable_init();
  if (!st) {
    printf("[FAIL] Failed to initialize symbol table for insertion test\n");
    return;
  }

  bool insert_result = symtable_insert_var(st, "myVar", TYPE_INT, true, false);
  if (!insert_result) {
    printf("[FAIL] Failed to insert variable symbol: myVar\n");
  } else {
    printf("[OK] Variable symbol insertion test passed for: myVar\n");
  }

  // check for douplicate insertion
  insert_result = symtable_insert_var(st, "myVar", TYPE_INT, true, false);
  if (insert_result) {
    printf("[FAIL] Unexpectedly succeeded in inserting duplicate variable "
           "symbol: myVar\n");
  } else {
    printf("[OK] Duplicate variable symbol insertion test passed for: myVar\n");
  }

  Symbol *found_sym = symtable_search_current_scope(st, "myVar");
  if (!found_sym || !matching_string(found_sym->name, "myVar") ||
      found_sym->d_type != TYPE_INT) {
    printf("[FAIL] Failed to find inserted variable symbol in current scope: "
           "myVar\n");
  } else {
    printf("[OK] Variable symbol search test passed for: myVar\n");
  }

  symtable_dispose(st);
}

void search_current_scope_test() {
  Symtable *st = symtable_init();
  if (!st) {
    printf("[FAIL] Failed to initialize symbol table for search test\n");
    return;
  }

  symtable_insert_var(st, "myVar", TYPE_INT, true, false);

  Symbol *found_sym = symtable_search_current_scope(st, "myVar");
  if (!found_sym || !matching_string(found_sym->name, "myVar") ||
      found_sym->d_type != TYPE_INT) {
    printf("[FAIL] Failed to find variable symbol in current scope: myVar\n");
  } else {
    printf("[OK] Variable symbol search in current scope test passed for: "
           "myVar\n");
  }

  symtable_dispose(st);
}

void search_in_all_scopes_test() {
  Symtable *st = symtable_init();
  if (!st) {
    printf("[FAIL] Failed to initialize symbol table for search in all scopes "
           "test\n");
    return;
  }

  symtable_insert_var(st, "myVar", TYPE_INT, true, false);
  scope_push(st);
  symtable_insert_var(st, "myVar2", TYPE_INT, true, false);

  Symbol *found_sym = symtable_search(st, "myVar");
  if (!found_sym || !matching_string(found_sym->name, "myVar") ||
      found_sym->d_type != TYPE_INT) {
    printf("[FAIL] Failed to find variable symbol in all scopes: myVar\n");
  } else {
    printf("[OK] Variable symbol search in all scopes test passed for: myVar "
           "from scope: %d\n",
           st->top);
  }

  found_sym = symtable_search(st, "myVar2");
  if (!found_sym || !matching_string(found_sym->name, "myVar2") ||
      found_sym->d_type != TYPE_INT) {
    printf("[FAIL] Failed to find variable symbol in all scopes: myVar2\n");
  } else {
    printf("[OK] Variable symbol search in all scopes test passed for: myVar2 "
           "from scope: %d\n",
           st->top);
  }

  scope_pop(st);

  symtable_dispose(st);
}

void function_insertion_and_search_test() {
  Symtable *st = symtable_init();
  if (!st) {
    printf("[FAIL] Failed to initialize symbol table for function insertion "
           "and search test\n");
    return;
  }

  symtable_insert_var(st, "myVar", TYPE_INT, true, false);
  Symbol *found_var = symtable_search(st, "myVar");
  if (found_var == NULL) {
    printf("[FAIL] Failed to find variable: myVar\n");
  } else if (found_var->mangled_name == NULL) {
    printf("[FAIL] Missing mangled name\n");
  } else if (strncmp(found_var->mangled_name, "GF@", 3) == 0) {
    printf("[FAIL] Variable symbol in main scope has incorrect level of frame: "
           "%s\n",
           found_var->mangled_name);
  } else {
    printf(
        "[OK] Variable symbol search in main scope test passed for: myVar\n");
  }

  Param params[2];
  params[0].name = "arg1";
  params[0].d_type = TYPE_INT;
  params[1].name = "arg2";
  params[1].d_type = TYPE_FLOAT;

  bool insert_result =
      symtable_insert_func(st, "myFunction", TYPE_VOID, params, 2);
  if (!insert_result) {
    printf("[FAIL] Failed to insert function symbol: myFunction\n");
  } else {
    printf("[OK] Function symbol insertion test passed for: myFunction\n");
  }

  Symbol *found_sym = symtable_search(st, "myFunction");
  if (!found_sym || !matching_string(found_sym->name, "myFunction") ||
      found_sym->d_type != TYPE_VOID || found_sym->params_amount != 2) {
    printf("[FAIL] Failed to find inserted function symbol: myFunction\n");
  } else {
    printf("[OK] Function symbol search test passed for: myFunction\n");
  }

  scope_push(st);
  symtable_insert_var(st, "myVar", TYPE_INT, true, false);
  found_var = symtable_search(st, "myVar");
  if (found_var == NULL) {
    printf("[FAIL] Failed to find variable: myVar\n");
  } else if (found_var->mangled_name == NULL) {
    printf("[FAIL] Missing mangled name\n");
  } else if (strncmp(found_var->mangled_name, "LF@", 3) != 0) {
    printf("[FAIL] Variable symbol in nested scope has incorrect frame: %s\n",
           found_var->mangled_name);
  } else {
    printf(
        "[OK] Variable symbol search in nested scope test passed for: myVar\n");
  }
  scope_pop(st);

  symtable_dispose(st);
}

void capacity_and_rehashing_test() {
  Symtable *st = symtable_init();
  if (!st) {
    printf("[FAIL] Failed to initialize symbol table for capacity and "
           "rehashing test\n");
    return;
  }

  // Insert 100 distinct variable symbols
  for (int i = 0; i < 100; i++) {
    char var_name[20];
    snprintf(var_name, sizeof(var_name), "v%d", i);
    bool insert_result =
        symtable_insert_var(st, var_name, TYPE_INT, true, false);
    if (!insert_result) {
      printf("[FAIL] Failed to insert variable symbol: %s\n", var_name);
      symtable_dispose(st);
      return;
    }
  }

  // Search for each of the 100 names
  for (int i = 0; i < 100; i++) {
    char var_name[20];
    snprintf(var_name, sizeof(var_name), "v%d", i);
    Symbol *found_sym = symtable_search(st, var_name);
    if (!found_sym || !matching_string(found_sym->name, var_name)) {
      printf("[FAIL] Failed to find variable symbol: %s\n", var_name);
      symtable_dispose(st);
      return;
    }
  }

  // Attempt to insert v100
  bool insert_result = symtable_insert_var(st, "v100", TYPE_INT, true, false);
  if (insert_result) {
    printf(
        "[FAIL] Unexpectedly succeeded in inserting variable symbol: v100\n");
  } else {
    printf("[OK] Correctly failed to insert variable symbol: v100 due to "
           "capacity limit\n");
  }

  // Push MAX_SCOPES - 1 more times
  for (int i = 0; i < MAX_SCOPES - 1; i++) {
    scope_push(st);
  }
  if (st->top != MAX_SCOPES - 1) {
    printf("[FAIL] Unexpected top value after pushing scopes: %d\n", st->top);
  } else {
    printf("[OK] Successfully pushed scopes up to MAX_SCOPES - 1\n");
  }

  // Push once more
  scope_push(st);
  if (st->top != MAX_SCOPES - 1) {
    printf("[FAIL] Unexpected top value after pushing one more scope: %d\n",
           st->top);
  } else {
    printf("[OK] Successfully handled pushing beyond MAX_SCOPES without "
           "crashing\n");
  }

  symtable_dispose(st);
}

void scope_push_and_pop_test() {
  Symtable *st = symtable_init();
  if (!st) {
    printf("[FAIL] Failed to initialize symbol table for scope push and pop "
           "test\n");
    return;
  }

  // Push a new scope
  scope_push(st);
  symtable_insert_var(st, "inner", TYPE_INT, true, false);
  Symbol *found_inner = symtable_search(st, "inner");
  if (!found_inner) {
    printf("[FAIL] Failed to find variable symbol in inner scope: inner\n");
  } else {
    printf("[OK] Successfully found variable symbol in inner scope: inner\n");
  }

  // Pop the inner scope
  scope_pop(st);
  Symbol *found_after_pop = symtable_search(st, "inner");
  if (found_after_pop) {
    printf("[FAIL] Unexpectedly found variable symbol after popping inner "
           "scope: inner\n");
  } else {
    printf("[OK] Successfully did not find variable symbol after popping inner "
           "scope: inner\n");
  }

  // Insert shared in outer scope
  symtable_insert_var(st, "shared", TYPE_INT, true, false);

  // Push again and insert shared in inner scope
  scope_push(st);
  symtable_insert_var(st, "shared", TYPE_FLOAT, true, false);

  // Pop and check that the outer shared is found
  scope_pop(st);
  Symbol *found_shared = symtable_search(st, "shared");
  if (!found_shared || found_shared->d_type != TYPE_INT) {
    printf("[FAIL] Failed to find outer shared variable after popping inner "
           "scope\n");
  } else {
    printf("[OK] Successfully found outer shared variable after popping inner "
           "scope\n");
  }
  symtable_dispose(st);
}

void double_function_insertion_test() {
  Symtable *st = symtable_init();
  if (!st) {
    printf("[FAIL] Failed to initialize symbol table for double function "
           "insertion test\n");
    return;
  }

  char *function_name = "f";

  if (symtable_insert_var(st, NULL, TYPE_INT, true, false) == false)
    printf("[OK] NULL variable name was rejected\n");
  else
    printf("[FAIL] NULL variable name was accepted\n");

  if (symtable_search(st, NULL) == NULL)
    printf("[OK] Search with NULL name returned NULL\n");
  else
    printf("[FAIL] Search with NULL name found a symbol\n");

  if (symtable_search_current_scope(st, NULL) == NULL)
    printf("[OK] Current-scope search with NULL name returned NULL\n");
  else
    printf("[FAIL] Current-scope search with NULL name found a symbol\n");

  if (symtable_insert_func(st, NULL, TYPE_VOID, NULL, 0) == false)
    printf("[OK] NULL function name was rejected\n");
  else
    printf("[FAIL] NULL function name was accepted\n");

  bool first_insert =
      symtable_insert_func(st, function_name, TYPE_VOID, NULL, 0);

  if (first_insert)
    printf("[OK] First insertion of function f succeeded\n");
  else
    printf("[FAIL] First insertion of function f failed\n");

  bool duplicate_insert =
      symtable_insert_func(st, function_name, TYPE_VOID, NULL, 0);

  if (!duplicate_insert)
    printf("[OK] Duplicate insertion of function f was rejected\n");
  else
    printf("[FAIL] Duplicate insertion of function f succeeded\n");

  Symbol *first = symtable_search(st, function_name);
  if (first == NULL) {
    printf("[FAIL] Function f was not found after insertion\n");
  } else if (first->s_type != SYMBOL_FUNC) {
    printf("[FAIL] f was inserted but is not SYMBOL_FUNC\n");
  } else {
    printf("[OK] Function f is still stored as SYMBOL_FUNC\n");
  }

  // Local variables may shadow functions.
  scope_push(st);

  bool shadow_insert =
      symtable_insert_var(st, function_name, TYPE_INT, true, false);

  if (shadow_insert)
    printf("[OK] Local variable f can shadow function f\n");
  else
    printf("[FAIL] Local variable f could not shadow function f\n");

  Symbol *local = symtable_search_current_scope(st, function_name);
  if (local == NULL)
    printf("[FAIL] Shadow variable f was not found in the inner scope\n");
  else if (local->s_type != SYMBOL_VAR)
    printf("[FAIL] Inner f is not SYMBOL_VAR\n");
  else
    printf("[OK] Inner f is a SYMBOL_VAR\n");

  scope_pop(st);

  Symbol *outer = symtable_search(st, function_name);
  if (outer == NULL)
    printf("[FAIL] Function f disappeared after popping the inner scope\n");
  else if (outer->s_type != SYMBOL_FUNC)
    printf("[FAIL] f after scope pop is not SYMBOL_FUNC\n");
  else
    printf("[OK] Function f is restored after scope pop\n");

  symtable_dispose(st);
}

int main() {
  // Test symbol creation and properties
  test_devider("Symbol Test");
  symbol_test("myVar", SYMBOL_VAR, TYPE_INT, true, false);

  // Test table initialization
  test_devider("Symbol Table Initialization Test");
  symtable_init_test();

  // Symbol insertion
  test_devider("Symbol Insertion Test");
  symbol_insertion_test();

  // Symbol search in current scope
  test_devider("Symbol Search in Current Scope Test");
  search_current_scope_test();

  // Scope push and pop test
  test_devider("Scope Push and Pop Test");
  scope_push_and_pop_test();

  // Symbol search in all scopes
  test_devider("Symbol Search in All Scopes Test");
  search_in_all_scopes_test();

  // function insertion and search test
  test_devider("Function Insertion and Search Test");
  function_insertion_and_search_test();

  // double function insertion test
  test_devider("Double Function Insertion Test");
  double_function_insertion_test();

  // Capacity and rehashing test
  test_devider("Capacity and Rehashing Test");
  capacity_and_rehashing_test();
  return 0;
}
