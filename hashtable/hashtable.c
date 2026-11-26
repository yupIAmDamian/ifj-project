// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)

#include "stdio.h"
#include "hashtable.h"
#include "stdlib.h"
#include "../helper_func.h"
#include <string.h>

int hash_function(char* key, int size){
    unsigned long hash = 0;
    int key_length = strlen(key);
    for(int i = 0; i < key_length; i++){
        hash += (unsigned long)key[i];
        hash = (hash * (unsigned long)key[i]) % size;
    }
    return ((int)hash) % size;
}

int probing(hashtable_t* hashtable, char* key, int attempt){
    int index = hash_function(key, hashtable->size);
    index = (index + attempt) % hashtable->size;
    if(hashtable->items[index].state == EMPTY){
        return index;
    }
    if(hashtable->items[index].state == OCCUPIED && matching_string(hashtable->items[index].key, key) == 0){
        return index;
    }
    if(attempt < hashtable->size-1){
        return probing(hashtable, key, attempt + 1);
    }
    return -1;
}

ht_item_t* ht_search(hashtable_t* hashtable, char* key){
    int index = probing(hashtable, key, 0);
    if(index != -1 && hashtable->items[index].state == OCCUPIED && matching_string(hashtable->items[index].key, key) == 0){
        return &hashtable->items[index];
    }
    return NULL;
}

ht_item_t* ht_insert(hashtable_t* hashtable, char* key, void* data){
    int index = probing(hashtable, key, 0);
    if(index == -1){
        return NULL;
    }

    if(hashtable->items[index].state == OCCUPIED && matching_string(hashtable->items[index].key, key) == 0){
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

bool ht_delete(hashtable_t* hashtable, char* key){
    ht_item_t* item = ht_search(hashtable, key);
    if(item == NULL) return false;

    item->state = DELETED;

    hashtable->count--;
    return true;
}

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

void ht_clear(hashtable_t** hashtable){
    if(hashtable == NULL || *hashtable == NULL){
        return;
    }

    free_keys((*hashtable)->items, (*hashtable)->size);
    free((*hashtable)->items);
    free(*hashtable);
    *hashtable = NULL;
}