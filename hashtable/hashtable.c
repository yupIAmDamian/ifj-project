// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)

#include "stdio.h"
#include "hashtable.h"
#include "stdlib.h"
#include "../helper_func.h"
#include <string.h>

/**
 * @brief Computes a hash index for a given string key.
 * @param key Null-terminated string key to hash.
 * @param size Capacity of the hash table.
 * @return Computed index in the range [0, size - 1].
 */
int hash_function(char* key, int size){
    unsigned long hash = 0;
    int key_length = strlen(key);
    for(int i = 0; i < key_length; i++){
        hash += (unsigned long)key[i];
        hash = (hash * (unsigned long)key[i]) % size;
    }
    return ((int)hash) % size;
}

/**
 * @brief Performs recursive linear probing to find a matching key or an available slot.
 * @param hashtable Pointer to the hash table.
 * @param key Null-terminated string key to locate.
 * @param attempt Current probing offset (starts at 0).
 * @return Index of the matching or empty slot, or -1 if the table is full.
 */
int probing(hashtable_t* hashtable, char* key, int attempt){
    int index = hash_function(key, hashtable->size);
    index = (index + attempt) % hashtable->size;
    if(hashtable->items[index].state == EMPTY){
        return index;
    }
    if(hashtable->items[index].state == OCCUPIED && strcmp(hashtable->items[index].key, key) == 0){
        return index;
    }
    if(attempt < hashtable->size-1){
        return probing(hashtable, key, attempt + 1);
    }
    return -1;
}

/**
 * @brief Searches for an occupied item matching the given key in the hash table.
 * @param hashtable Pointer to the hash table.
 * @param key Null-terminated string key to search for.
 * @return Pointer to the matching hash table item, or NULL if not found.
 */
ht_item_t* ht_search(hashtable_t* hashtable, char* key){
    int index = probing(hashtable, key, 0);
    if(index != -1 && hashtable->items[index].state == OCCUPIED && strcmp(hashtable->items[index].key, key) == 0){
        return &hashtable->items[index];
    }
    return NULL;
}

/**
 * @brief Inserts a new key-value pair into the hash table.
 * @param hashtable Pointer to the hash table.
 * @param key Null-terminated string key to insert (duplicated internally).
 * @param data Generic pointer to the associated payload data.
 * @return Pointer to the inserted hash table item, or NULL if the table is full, key exists, or allocation fails.
 */
ht_item_t* ht_insert(hashtable_t* hashtable, char* key, void* data){
    int index = probing(hashtable, key, 0);
    if(index == -1){
        return NULL;
    }

    if(hashtable->items[index].state == OCCUPIED && strcmp(hashtable->items[index].key, key) == 0){
        return NULL;
    }

    char* hash_item_key = str_duplicate(key);
    if(hash_item_key == NULL) {
        return NULL;
    }
    hashtable->items[index].key = hash_item_key;
    hashtable->items[index].state = OCCUPIED;
    hashtable->items[index].data = data;

    hashtable->count++;
    return &hashtable->items[index];
}

/**
 * @brief Marks an existing item with the given key as deleted (tombstone).
 * @param hashtable Pointer to the hash table.
 * @param key Null-terminated string key of the item to delete.
 * @return true if the item was found and marked deleted, false otherwise.
 */
bool ht_delete(hashtable_t* hashtable, char* key){
    ht_item_t* item = ht_search(hashtable, key);
    if(item == NULL) return false;

    item->state = DELETED;

    hashtable->count--;
    return true;
}

/**
 * @brief Allocates and initializes a new hash table of the given capacity.
 * @param size Number of slots to allocate in the hash table.
 * @return Pointer to the newly created hash table, or NULL on allocation failure.
 */
hashtable_t* ht_create(int size){
    hashtable_t* hashtable = malloc(sizeof(hashtable_t));
    if(hashtable == NULL){
        return NULL;
    }

    hashtable->items = calloc(size, sizeof(ht_item_t));
    if(hashtable->items == NULL){
        free(hashtable);
        return NULL;
    }

    hashtable->size = size;
    hashtable->count = 0;

    return hashtable;
}

/**
 * @brief Frees all dynamically allocated keys stored in the hash table item array.
 * @param items Pointer to the array of hash table items.
 * @param size Number of slots in the item array.
 * @return true if keys were freed successfully, false if items is NULL.
 */
bool free_keys(ht_item_t* items, int size) {
    if (items == NULL) {
        return false;
    }

    for (int i = 0; i < size; i++) {
        if ((items[i].state == OCCUPIED || items[i].state == DELETED) && items[i].key != NULL) {
            free(items[i].key);
            items[i].key = NULL;
        }
    }
    return true;
}

/**
 * @brief Frees all memory allocated for the hash table structure, its item array, and its keys.
 * @param hashtable Double pointer to the hash table to dispose of (set to NULL afterward).
 */
void ht_clear(hashtable_t** hashtable){
    if(hashtable == NULL || *hashtable == NULL){
        return;
    }

    free_keys((*hashtable)->items, (*hashtable)->size);
    free((*hashtable)->items);
    free(*hashtable);
    *hashtable = NULL;
}