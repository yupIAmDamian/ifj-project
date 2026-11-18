// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)

#ifndef HASH_TABLE_TESTS_H
#define HASH_TABLE_TESTS_H
#include "../../hashtable/hashtable.h"

void insert_test(hashtable_t* hashtable, char* key, void* data);
void search_test(hashtable_t* hashtable, char* key);
void delete_test(hashtable_t* hashtable, char* key);
void cleanup(hashtable_t** hashtable);
int main();

#endif