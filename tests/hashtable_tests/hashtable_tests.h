// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)

#ifndef HASH_TABLE_TESTS_H
#define HASH_TABLE_TESTS_H
#include "../../hashtable/hashtable.h"

/**
 * @brief Tests inserting a key-value pair into the hash table and prints an error message on failure.
 * @param hashtable Pointer to the hash table.
 * @param key String key to insert.
 * @param data Pointer to the data associated with the key.
 */
void insert_test(hashtable_t* hashtable, char* key, void* data);

/**
 * @brief Tests searching for a key in the hash table and prints the result.
 * @param hashtable Pointer to the hash table.
 * @param key String key to search for.
 */
void search_test(hashtable_t* hashtable, char* key);

/**
 * @brief Tests deleting a key from the hash table and prints whether the operation succeeded.
 * @param hashtable Pointer to the hash table.
 * @param key String key to delete.
 */
void delete_test(hashtable_t* hashtable, char* key);

/**
 * @brief Frees all allocated keys, item array, and the hash table structure, setting the pointer to NULL.
 * @param hashtable Double pointer to the hash table to deallocate.
 */
void cleanup(hashtable_t** hashtable);

/**
 * @brief Entry point that executes unit tests for hash table insertion, searching, deletion, probing, and error handling.
 * @return 0 upon successful test execution.
 */
int main();

#endif