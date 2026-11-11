#include <stddef.h>
#include <stdio.h>
#include "hashtable_func.h"
#include "../../hashtable/hashtable.h"

/*
TODO: Add more tests for edge cases and error handling, including:
Hash collisions correctly.
Inserting into a full table.
Deleting nonexistent keys.
Repeated insertions and deletions.
NULL pointers.
Invalid input parameters.
Double-free errors in code paths your tests don't execute.
*/

int hashtable_tests() {
    hashtable_t* hashtable = ht_create(10);
    if (hashtable == NULL) {
        printf("Failed to create hashtable\n");
        return 1;
    }

    int data1 = 42;
    ht_insert(hashtable, "key1", &data1);

    int data2 = 84;
    ht_insert(hashtable, "key2", &data2);

    ht_item_t* item = ht_search(hashtable, "key1");
    if (item != NULL) {
        printf("Found key1: %d\n", *(int*)item->data);
    } else {
        printf("key1 not found\n");
    }

    ht_delete(hashtable, "key1");

    item = ht_search(hashtable, "key1");
    if (item != NULL) {
        printf("Found key1: %d\n", *(int*)item->data);
    } else {
        printf("key1 not found after deletion\n");
    }

    ht_clear(&hashtable);

    if (hashtable != NULL) {
        printf("Hashtable not properly cleared\n");
    }

    return 0;
}
