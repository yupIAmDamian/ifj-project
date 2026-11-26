Pravidla na testovanie:

1. testujeme kazdu funkciu ktoru vytvorime
2. funkciu zavolame a dame do nej vstup pre ktory vieme vystup
3. nechame ju bezat
4. porovname jej vystup s vystupom ktory ocakavame
5. vypiseme chybove hlasky

priklad:
hashtable_t\* hashtable = ht_create(10);
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
