typedef enum state_enum {
    EMPTY,
    OCCUPIED,
    DELETED
} state_enum;

typedef struct  ht_item_t{
    char* key;
    char* data;
    state_enum state;// State of the item
} ht_item_t;

typedef struct hashtable_t{
    ht_item_t* items;
    int size;
    int count;
}hashtable_t;

//hash
int hash_function(const char* key, int size);
int probing(hashtable_t* hashtable, const char* key, int attempt);

hashtable_t* ht_create(int size);
hashtable_t* ht_search(hashtable_t* hashtable, const char* key);
hashtable_t* ht_delete(hashtable_t* hashtable, const char* key);
hashtable_t* ht_insert(hashtable_t* hashtable, const char* key, const char* data);
//frees all allocated hastable memory
hashtable_t* ht_clear(hashtable_t* hashtable);