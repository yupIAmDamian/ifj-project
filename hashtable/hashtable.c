// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)

#include "stdio.h"
#include "hashtable.h"
#include "stdlib.h"
#include "../helper_func.h"

//TODO: apply DRY principle 

int hash_function(char* key, int size){
    unsigned long hash = 0;
    int key_length = strlen(key);
    for(int i = 0; i < key_length; i++){
        hash += (unsigned long)key[i];
        hash = (hash * (unsigned long)key[i]) % size;
    }
    return (int)hash;
}

int probing(hashtable_t* hashtable, char* key, int attempt){
    int index = hash_function(key, hashtable->size);
    if(hashtable->items[index].state == EMPTY){
        return index;
    }
    if(hashtable->items[index].state == OCCUPIED && strcmp(hashtable->items[index].key, key) == 0){
        return index;
    }
    if(attempt < hashtable->size){
        return probing(hashtable, key, attempt + 1);
    }
    return -1;
}

ht_item_t* ht_search(hashtable_t* hashtable, char* key){
    //check this is good. its a bit sus
    int index = probing(hashtable, key, 0);
    if(index == -1){
        return NULL;
    }

    return &hashtable->items[index];
}

ht_item_t* ht_insert(hashtable_t* hashtable, char* key, void* data){
    int index = probing(hashtable, key, 0);
    if(index == -1){
        return NULL;
    }

    hashtable->items[index].state = OCCUPIED;
    hashtable->items[index].data = data;
    hashtable->items[index].key = str_duplicate(key);

    hashtable->count++;
    return &hashtable->items[index];
}


hashtable_t* ht_delete(hashtable_t* hashtable, char* key){
    //checknut ci toto je dobre. je to trosku sus
    int index = probing(hashtable, key, 0);
    if(index == -1){
        return NULL;
    }

    hashtable->items[index].state = DELETED;
    hashtable->items[index].data = NULL;

    hashtable->count--;
    return hashtable;
}

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

    hashtable->size = size;
    hashtable->count = 0;

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