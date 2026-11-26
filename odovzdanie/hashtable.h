// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdbool.h>

/**
 * @brief Represents the state of a slot in the open-addressing hash table.
 */
typedef enum state_enum { EMPTY, OCCUPIED, DELETED } state_enum;

/**
 * @brief Represents a single key-value entry in the hash table.
 */
typedef struct ht_item_t {
  char *key;        // Dynamically allocated identifier string
  void *data;       // Generic pointer to stored payload (e.g., Symbol*)
  state_enum state; // Current state of the hash table slot
} ht_item_t;

/**
 * @brief Represents an open-addressing hash table with linear probing.
 */
typedef struct hashtable_t {
  ht_item_t *items; // Array of hash table slots
  int size;         // Total allocated capacity of the table
  int count;        // Number of currently occupied entries
} hashtable_t;

/**
 * @brief Computes a hash index for a given string key.
 * @param key Null-terminated string key to hash.
 * @param size Capacity of the hash table.
 * @return Computed index in the range [0, size - 1].
 */
int hash_function(char *key, int size);

/**
 * @brief Performs recursive linear probing to find a matching key or an
 * available slot.
 * @param hashtable Pointer to the hash table.
 * @param key Null-terminated string key to locate.
 * @param attempt Current probing offset (starts at 0).
 * @return Index of the matching or empty slot, or -1 if the table is full.
 */
int probing(hashtable_t *hashtable, char *key, int attempt);

/**
 * @brief Allocates and initializes a new hash table of the given capacity.
 * @param size Number of slots to allocate in the hash table.
 * @return Pointer to the newly created hash table, or NULL on allocation
 * failure.
 */
hashtable_t *ht_create(int size);

/**
 * @brief Searches for an occupied item matching the given key in the hash
 * table.
 * @param hashtable Pointer to the hash table.
 * @param key Null-terminated string key to search for.
 * @return Pointer to the matching hash table item, or NULL if not found.
 */
ht_item_t *ht_search(hashtable_t *hashtable, char *key);

/**
 * @brief Marks an existing item with the given key as deleted (tombstone).
 * @param hashtable Pointer to the hash table.
 * @param key Null-terminated string key of the item to delete.
 * @return true if the item was found and marked deleted, false otherwise.
 */
bool ht_delete(hashtable_t *hashtable, char *key);

/**
 * @brief Inserts a new key-value pair into the hash table.
 * @param hashtable Pointer to the hash table.
 * @param key Null-terminated string key to insert (duplicated internally).
 * @param data Generic pointer to the associated payload data.
 * @return Pointer to the inserted hash table item, or NULL if the table is
 * full, key exists, or allocation fails.
 */
ht_item_t *ht_insert(hashtable_t *hashtable, char *key, void *data);

/**
 * @brief Frees all dynamically allocated keys stored in the hash table item
 * array.
 * @param items Pointer to the array of hash table items.
 * @param size Number of slots in the item array.
 * @return true if keys were freed successfully, false if items is NULL.
 */
bool free_keys(ht_item_t *items, int size);

/**
 * @brief Frees all memory allocated for the hash table structure, its item
 * array, and its keys.
 * @param hashtable Double pointer to the hash table to dispose of (set to NULL
 * afterward).
 */
void ht_clear(hashtable_t **hashtable);

#endif