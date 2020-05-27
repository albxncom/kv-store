// libraries
#include <stdio.h>
#include <stdlib.h>
#include "hash_table.h"

int main() {

    hash_table *my_table;

    // Exit program if initialization failed
    if(init_ht(&my_table, 997, 0, NULL) == 1) exit(1);

    char *key = "hello";
    char *value = "world";

    // Add
    add_ht(my_table, key, (void *) value, strlen(value));

    // Update
    char *new_value = "world!";
    int upd = upd_ht(my_table, key, (void *) new_value, strlen(new_value));

    // Get
    void *get_val;
    size_t *get_val_len;
    int get = get_ht(my_table, key, &get_val, &get_val_len);
    if(get == 0) printf("We got: %s (length: %lu)\n", (char *) get_val, *get_val_len);

    // Remove
    rem_ht(my_table, key);

    free_ht(my_table);


    return 0;
}