#include "stdio.h"
#include "hashtable.h"

/*
int hash_function(const char* key, int size);
int probing(hashtable_t* hashtable, const char* key, int attempt);


hashtable_t* ht_search(hashtable_t* hashtable, const char* key);
hashtable_t* ht_delete(hashtable_t* hashtable, const char* key);
hashtable_t* ht_insert(hashtable_t* hashtable, const char* key, const char* data);
*/

//creates a new hashtable
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

    hashtable-> size = size;
    hashtable-> count = 0;

    return hashtable;
}

//frees all allocated hastable memory
void ht_clear(hashtable_t** hashtable){
    if(hashtable == NULL || *hashtable == NULL){
        return;
    }

    if((*hashtable)->items != NULL){
        for(int item_index =0; item_index < (*hashtable)->size; item_index++){
            free((*hashtable)->items[item_index].key);
            free((*hashtable)->items[item_index].data);
        }
    }

    free((*hashtable)->items);
    free(*hashtable);
    *hashtable = NULL;
}