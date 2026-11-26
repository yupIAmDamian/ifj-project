// Compiler of IFJ26 imperative language
// Author: Damian Frantisek Girasek (xgirasd00)
#include "../../hashtable/hashtable.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void insert_test(hashtable_t *hashtable, char *key, void *data) {
  ht_item_t *item = ht_insert(hashtable, key, data);
  if (item == NULL) {
    printf("Failed to insert key: %s\n", key);
  }
}

void search_test(hashtable_t *hashtable, char *key) {
  ht_item_t *item = ht_search(hashtable, key);
  if (item != NULL) {
    printf("Found key: %s, data: %s\n", item->key, (char *)item->data);
  } else {
    printf("Key not found: %s\n", key);
  }
}

void delete_test(hashtable_t *hashtable, char *key) {
  bool success = ht_delete(hashtable, key);
  if (success) {
    printf("Deleted key: %s\n", key);
  } else {
    printf("Failed to delete key: %s\n", key);
  }
}

void cleanup(hashtable_t **hashtable) {
  if (hashtable == NULL || *hashtable == NULL) {
    return;
  }

  if ((*hashtable)->items != NULL) {
    for (int item_index = 0; item_index < (*hashtable)->size; item_index++) {
      free((*hashtable)->items[item_index].key);
    }
  }

  free((*hashtable)->items);
  free(*hashtable);
  *hashtable = NULL;
}

int main() {
  hashtable_t *hashtable = ht_create(3);

  // Probing test
  insert_test(hashtable, "key1", "data1");
  insert_test(hashtable, "key2", "data2");
  insert_test(hashtable, "key3", "data3");
  delete_test(hashtable, "key3");

  // Fail tests
  // blinding
  insert_test(hashtable, "key3", "data3");

  // Duplication
  insert_test(hashtable, "key1", "data1");

  // Overflow
  insert_test(hashtable, "key4", "data4");
  cleanup(&hashtable);

  // Search test
  hashtable = ht_create(3);
  insert_test(hashtable, "key1", "data1");
  insert_test(hashtable, "key2", "data2");

  search_test(hashtable, "key1");
  search_test(hashtable, "key2");

  // Fail test
  search_test(hashtable, "key3");

  cleanup(&hashtable);

  // Delete test
  hashtable = ht_create(2);
  insert_test(hashtable, "key1", "data1");
  search_test(hashtable, "key1");

  // Fail test
  delete_test(hashtable, "key2");

  delete_test(hashtable, "key1");

  search_test(hashtable, "key1");

  // Fail test
  delete_test(hashtable, "key1");

  cleanup(&hashtable);

  return 0;
}
