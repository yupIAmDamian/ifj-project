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
int hash_function(const char* key, int size);
int probing(hashtable_t* hashtable, const char* key, int attempt);

hashtable_t* ht_create(int size);
ht_item_t* ht_search(hashtable_t* hashtable, const char* key);
//Should we return 1 for success and 0 for failure? or return hashtable?
hashtable_t* ht_delete(hashtable_t* hashtable, const char* key);
ht_item_t* ht_insert(hashtable_t* hashtable, const char* key, const void* data);
//frees all allocated hastable memory
void ht_clear(hashtable_t** hashtable);