// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)

#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <stdbool.h> 

typedef enum state_enum {
    EMPTY,
    OCCUPIED,
    DELETED
} state_enum;

typedef struct  ht_item_t{
    char* key;
    void* data;//temporary void until we declare token type
    state_enum state;// State of the item
} ht_item_t;

typedef struct hashtable_t{
    ht_item_t* items;
    int size;
    int count;
}hashtable_t;

//hash
int hash_function(char* key, int size);
int probing(hashtable_t* hashtable, char* key, int attempt);

hashtable_t* ht_create(int size);
ht_item_t* ht_search(hashtable_t* hashtable, char* key);


bool ht_delete(hashtable_t* hashtable, char* key);
ht_item_t* ht_insert(hashtable_t* hashtable, char* key, void* data);

bool free_keys(ht_item_t* items, int size);
//frees all allocated hastable memory
void ht_clear(hashtable_t** hashtable);

#endif