#include "hash_table.h"


/**
 * Initialize the hash table
 * @param   table               Pointer to pointer of hash_table.
 * @param   size                Size of the hash_table. Be sure to take here a value of 2^n or prime numbers.
 * @param   val_printable       Indicates whether the value is printable, 
 *                              HT_VAL_PRINTABLE = 1 = yes, HT_VAL_NOT_PRINTABLE = 0 = no 
 * @param   (*val_print_fn)())  Pointer to function which returns a string to be printed, 
 *                              see sprint_val_str() or sprint_val_int() for examples.
 * @return                      0 on success, 1 on failure.
 */
int init_ht(hash_table **table, int size, int val_printable, void (*val_print_fn)()) {

    // Try to malloc
    hash_table *_table = (hash_table *) malloc(sizeof(hash_table));
    if(DEBUG_HASH_TABLE_MEMLEAK) printf("debug: hash_table: memleak: malloc()ed table @ %p\n", _table);
    bucket_list *data = (bucket_list *) malloc(sizeof(bucket_list) * size);
    if(DEBUG_HASH_TABLE_MEMLEAK) printf("debug: hash_table: memleak: malloc()ed data @ %p\n", data);

    // Error
    if(_table == NULL || data == NULL) {
        if(DEBUG_HASH_TABLE_MEMLEAK) printf("debug: hash_table: error, hash_table could not be malloc()ed.\n");
        return 1;
    }

    // Successfully malloc()ed. Feed it.
    _table->data = data;
    _table->count = 0;
    _table->collisions = 0;
    _table->size = size;
    _table->val_printable = val_printable;
    _table->val_print_fn = val_print_fn;

    // Zero data
    memset(_table->data, 0, sizeof(bucket_list) * size);

    *table = _table;

    return 0;
}


/**
 * Given a table and a key, this function will return a pointer to a node
 * with the same key in the hash table.
 * @param  table Pointer to table
 * @param  key   The key
 * @return       Pointer to node with the same key or NULL if it cant be found.
 */
hash_node *get_el_in_bucket_list(hash_table *table, char *key) {

    // Hash the value
    unsigned long hashed_val = hash(table, key);

    // Define here our last node in bucket_list
    hash_node **last_node_in_bucket_list;
    last_node_in_bucket_list = &(table->data + hashed_val)->node;

    // If count <= 0 => Bucket empty or undefined behaviour, return NULL
    if((table->data + hashed_val)->count <= 0) {
        // fix undefined behaviour otherwise replace 0 by 0
        (table->data + hashed_val)->count = 0;
        return *last_node_in_bucket_list;
    }

    // Check if there is already something there, i.e. if count of bucket list is > 0
    if((table->data + hashed_val)->count > 0) {
        // If yes, traverse path until key of node equals given key
        while((*last_node_in_bucket_list)->next != NULL && (*last_node_in_bucket_list)->key != NULL && strcmp((*last_node_in_bucket_list)->key, key) != 0) {
            last_node_in_bucket_list = &(*last_node_in_bucket_list)->next;
        }
    }

    return *last_node_in_bucket_list;
}


/**
 * Add an (key,val) pair to the hash table.
 * @param   table     Pointer to a hash_table
 * @param   key       The key
 * @param   value     Pointer to a pointer of value.
 * @param   val_len   The size of the value.
 * @return            0 on success, 1 on failure.
 */
int add_ht(hash_table *table, char *key, void **val, size_t val_len) {

    if(DEBUG_HASH_TABLE) printf("---------------------- debug add(%s) ----------------------\n", key);

    // Hash the value
    unsigned long hashed_val = hash(table, key);

    // Check if key exists, if yes return 1 = failure
    hash_node *if_key_exists = get_el_in_bucket_list(table, key);

    if(if_key_exists != NULL && if_key_exists->key != NULL && strcmp(if_key_exists->key, key) == 0) { // If keys are the same
        if(DEBUG_HASH_TABLE) printf("debug: hash_table: could not be inserted, key exists already!\n");
        return 1;
    }

    // Define here our last node in bucket_list. It's not that yet, we'll have to loop
    // through until the end of the bucket list is reached.
    hash_node **last_node_in_bucket_list;
    last_node_in_bucket_list = &(table->data + hashed_val)->node;

    // Check if there is already something there, i.e. if count of bucket list is > 0
    if((table->data + hashed_val)->count > 0) {

        // If yes, traverse path until next == NULL reached
        while((*last_node_in_bucket_list)->next != NULL) {
            last_node_in_bucket_list = &(*last_node_in_bucket_list)->next;
        }

        last_node_in_bucket_list = &(*last_node_in_bucket_list)->next;
    }

    // Allocate memory for struct_hash node
    *last_node_in_bucket_list = (hash_node *) malloc(sizeof(hash_node));
    if(DEBUG_HASH_TABLE_MEMLEAK) printf("debug: hash_table: memleak: malloc()ed node @ %p\n", (*last_node_in_bucket_list));
    if(last_node_in_bucket_list == NULL) {
        return 1;
    }

    // Allocate memory for node->key
    char *key_allocated = (char *) malloc(strlen(key));
    if(key_allocated == NULL) return 1; // Error
    if(DEBUG_HASH_TABLE_MEMLEAK) printf("debug: hash_table: memleak: malloc()ed key %p @ node %p\n", key_allocated, (*last_node_in_bucket_list));

    (*last_node_in_bucket_list)->key = key_allocated;
    strcpy((*last_node_in_bucket_list)->key, key); // Copy Key

    // Allocate memory for node->val
    void **val_allocated = (void **) malloc(val_len);
    if(val_allocated == NULL) return 1; // Error
    if(DEBUG_HASH_TABLE_MEMLEAK) printf("debug: hash_table: memleak: malloc()ed val %p @ node %p\n", val_allocated, (*last_node_in_bucket_list));


    // Copy val
    memcpy(val_allocated, val, val_len);
    (*last_node_in_bucket_list)->val = val_allocated;

    // Feed rest of data
    (*last_node_in_bucket_list)->val_len = val_len;
    (*last_node_in_bucket_list)->next = NULL; // Set next node to NULL

    // Update counters

    // And update our counter to keep track of how many elements we have in our table
    table->count++;

    // Update our collisions counter, but only if it's not the first element we add
    if((table->data + hashed_val)->count > 0) {
        table->collisions++;
    }

    // Finally update our bucket list counter
    (table->data + hashed_val)->count++; // Increase bucket list counter

    // Print Debug
    if(DEBUG_HASH_TABLE) {
        if(table->val_print_fn != NULL && table->val_printable) {
            char string[(*last_node_in_bucket_list)->val_len];
            (*table->val_print_fn)(&string[0], (*last_node_in_bucket_list)->val);
            printf("debug: hash_table: inserted at bucket_list[%lu] (new n: %i): (%s, %s) @ %p\n", hashed_val, (table->data + hashed_val)->count, (*last_node_in_bucket_list)->key, string, (*last_node_in_bucket_list));
        } else {
            printf("debug: hash_table: inserted at bucket_list[%lu] (new n: %i): (%s) @ %p\n", hashed_val, (table->data + hashed_val)->count, (*last_node_in_bucket_list)->key, (*last_node_in_bucket_list));
        }
    }

    if(DEBUG_HASH_TABLE) printf("------------------------------------------------------------------\n\n");

    return 0;
}


/**
 * Update an item in the hash_table
 * @param  table    Pointer to table
 * @param  key      The key for which the value will be updated
 * @param  value    Pointer to a pointer of a new value.
 * @param  val_len  The size of the new value.
 * @return          0 on success, 1 on failure.
 */
int upd_ht(hash_table *table, char *key, void **val, size_t val_len) {

    if(DEBUG_HASH_TABLE) printf("---------------------- debug update(%s) ----------------------\n", key);

    // Get the last node in bucket_list for the given key
    hash_node *last_node_in_bucket_list = get_el_in_bucket_list(table, key);

    // If key doesn't exist
    if(last_node_in_bucket_list->key == NULL || strcmp(last_node_in_bucket_list->key, key) != 0) {
        if(DEBUG_HASH_TABLE) printf("debug: hash_table: could not update, key does not exist.\n");
        return 1;
    }

    // Remove old element
    if(rem_ht(table, key) == 0){
        // Add new element
        add_ht(table, key, (void *) val, val_len);
    }



    if(DEBUG_HASH_TABLE) printf("------------------------------------------------------------------\n\n");

    return 0;
}


/**
 * Removes the node with for the given key
 * @param  table Pointer to the hash table
 * @param  key   Key
 * @return       0 on succcess, 1 on failure.
 */
int rem_ht(hash_table *table, char *key) {

    if(DEBUG_HASH_TABLE) printf("---------------------- debug remove(%s) ----------------------\n", key);

    // Hash the value
    unsigned long hashed_val = hash(table, key);

    // If there are no elements, then the key does not exist in our bucket list. return failure
    if((table->data + hashed_val)->count <= 0) {
        if(DEBUG_HASH_TABLE) printf("debug: hash_table: key %s could not be deleted, because it does not exist.\n", key);
        return 1;
    }

    // Now, let's look for right node, i.e. traverse the linked list
    // until the node with the same key is found

    // Define here our last node in bucket_list, which we're going to update until the right one is found
    hash_node *last_node_in_bucket_list;
    last_node_in_bucket_list = (table->data + hashed_val)->node;

    // On success, our if/else tree here will set success = 1.
    // This will be used to free later our malloc()s.
    int success = 0;

    // Define our node to be free()d later
    hash_node *node_to_free;

    // If there are at least 2 nodes stored in bucket list
    if((table->data + hashed_val)->count > 1) {

        // If the desired node is the first node in the list,
        // free it and repoint the pointer of the bucket list (*->node) to the next one
        if(last_node_in_bucket_list->key != NULL && strcmp(last_node_in_bucket_list->key, key) == 0) {

            if(DEBUG_HASH_TABLE) printf("debug: hash_table: deleting first element in bucket list of size bigger than 1\n");

            node_to_free = last_node_in_bucket_list;

            // Repoint bucket list pointer to next node;
            (table->data + hashed_val)->node = last_node_in_bucket_list->next;
        }

        // If the desired element is deeper in the bucket list and not the first one, traverse it
        else {


            while(last_node_in_bucket_list->next->next != NULL && strcmp(last_node_in_bucket_list->next->key, key) != 0) {
                last_node_in_bucket_list = last_node_in_bucket_list->next;
            }

            if(last_node_in_bucket_list->key != NULL && strcmp(last_node_in_bucket_list->next->key, key) == 0) {

                if(DEBUG_HASH_TABLE) printf("debug: hash_table: deleting not first element in bucket list of size bigger than 1\n");

                node_to_free = last_node_in_bucket_list->next;

                // Repoint next pointer
                last_node_in_bucket_list->next = last_node_in_bucket_list->next->next;
            }

            else {
                if(DEBUG_HASH_TABLE) printf("debug: hash_table: key %s could not be deleted, because it does not exist.\n", key);
                return 1;
            }

        }

    }

    // count == 1, now check if the keys are the same.
    else {

        // Check if key is the same as desired
        if(last_node_in_bucket_list->key != NULL && strcmp(last_node_in_bucket_list->key, key) == 0) {
            if(DEBUG_HASH_TABLE) printf("debug: hash_table: deleting first and only element in bucket list of size 1\n");

            node_to_free = last_node_in_bucket_list;

            // Reset pointer of bucket list and set success to 1
            (table->data + hashed_val)->node = NULL;
        }

        // If not, then the key to be deleted does not exist in our bucket list.
        else {
            if(DEBUG_HASH_TABLE) printf("debug: hash_table: key %s could not be deleted, because it does not exist.\n", key);
            return 1;
        }

    }

    // If we survived until here, this means that there is a node to be deleted.
    // Repointing happens above. If it could not repoint as it should, then the function returns already earliert with a 1.

    // Print Debug
    if(DEBUG_HASH_TABLE) {
        unsigned long hashed_val = hash(table, node_to_free->key);
        if(table->val_print_fn != NULL && table->val_printable) {
            char string[node_to_free->val_len];
            (*table->val_print_fn)(&string[0], node_to_free->val);
            printf("debug: hash_table: delete item at bucket_list[%lu] (new n: %i): (%s, %s) @ %p\n", hashed_val, (table->data + hashed_val)->count - 1, node_to_free->key, string, node_to_free);
        } else {
            printf("debug: hash_table: delete item at bucket_list[%lu] (new n: %i): (%s) @ %p\n", hashed_val, (table->data + hashed_val)->count - 1, node_to_free->key, node_to_free);
        }
    }

    // Free the node and print debug
    free(node_to_free->key);
    if(DEBUG_HASH_TABLE_MEMLEAK) printf("debug: hash_table: memleak: free()ed key %p @ node %p\n", node_to_free->key, node_to_free);
    free(node_to_free->val);
    if(DEBUG_HASH_TABLE_MEMLEAK) printf("debug: hash_table: memleak: free()ed val %p @ node %p\n", node_to_free->val, node_to_free);
    free(node_to_free);
    if(DEBUG_HASH_TABLE_MEMLEAK) printf("debug: hash_table: memleak: free()ed node %p\n", node_to_free);

    // Decrease the counter
    (table->data + hashed_val)->count--;
    table->count--;

    if(DEBUG_HASH_TABLE) printf("------------------------------------------------------------------\n\n");

    return 0;

}


/**
 * Get the value for a given key from the hash table. Pass the address to a pointer in val.
 * On success, the value of *val will point to the result. On failure, *val will be set
 * to NULL.
 * @param  table    Pointer to hash table
 * @param  key      The key
 * @param  val      Pointer to a pointer where the found value will be stored
 * @param  val_len  The size of the new value.
 * @return          0 on success, 1 on failure.
 */
int get_ht(hash_table *table, char *key, void **val, size_t **val_len) {

    if(DEBUG_HASH_TABLE) printf("---------------------- debug get(%s) ----------------------\n", key);

    hash_node *last_node_in_bucket_list = get_el_in_bucket_list(table, key);

    if(last_node_in_bucket_list == NULL || (last_node_in_bucket_list->key != NULL && strcmp(last_node_in_bucket_list->key, key) != 0)) { // If keys not equal, then empty
        if(DEBUG_HASH_TABLE) printf("debug: hash_table: could not get, key `%s` does not exist.\n", key);
        *val = NULL;
        return 1;
    }

    *val = last_node_in_bucket_list->val;
    *val_len = &last_node_in_bucket_list->val_len;


    unsigned long hashed_val = hash(table, key);

    // Print Debug
    if(DEBUG_HASH_TABLE) {
        if(table->val_print_fn != NULL && table->val_printable) {
            char string[last_node_in_bucket_list->val_len];
            (*table->val_print_fn)(&string[0], last_node_in_bucket_list->val);
            printf("debug: hash_table: get at bucket_list[%lu] (n: %i): (%s, %s) @ %p\n", hashed_val, (table->data + hashed_val)->count, last_node_in_bucket_list->key, string, last_node_in_bucket_list);
        } else {
            printf("debug: hash_table: get at bucket_list[%lu] (n: %i): (%s) @ %p\n", hashed_val, (table->data + hashed_val)->count, last_node_in_bucket_list->key, last_node_in_bucket_list);
        }
    }

    if(DEBUG_HASH_TABLE) printf("------------------------------------------------------------------\n\n");

    return 0;
}


/**
 * This function will store the contents of a hash_table to a file.
 * @param   table       The hash_table to store
 * @param   file        The file to write the contents into.
 * @param   delimeter   The delimeter between key and value which is going 
 *                      to be used in the text file.
 * @return              0 on success, 1 on failure.
 */
int store_ht(hash_table *table, char *file, char *delimeter) {

    if(table->val_printable) {

        // Open file
        FILE *fp;
        fp = fopen(file, "w");

        // For every line in table..
        for (int i = 0; i < table->size; ++i) {
            if((table->data + i)->count > 0) { // i.e. bucket list not empty

                // Print line
                hash_node *first_node_in_bucket_list = (table->data + i)->node;
                char string[((table->data + i)->node)->val_len];
                (*table->val_print_fn)(&string[0], ((table->data + i)->node)->val);
                fprintf(fp, "%s%s%s\n", first_node_in_bucket_list->key, delimeter, string);

                // If more than 1 node in bucket_list
                hash_node *traveler;
                if ((table->data + i)->count > 1) {
                    traveler = ((table->data + i)->node)->next;
                    while(traveler != NULL) {
                        (*table->val_print_fn)(&string[0], traveler->val);
                        fprintf(fp, "%s%s%s\n", traveler->key, delimeter, string);
                        traveler = traveler->next;
                    }
                }

            }
        }

        // Save file
        fclose(fp);
    }

    return 0;
}


/**
 * Loads the contentes of a file to the hash_table table
 * @param  table       The hash_table to load the content into
 * @param  file        The file from where the content is taken
 * @param  type        The type to store. Currently either int: HT_TYPE_INT = 0 or str: HT_TYPE_STR = 1
 * @param  delimeter   The delimeter between key and value which is going 
 *                     to be used in the text file.
 * @return             0 on success, 1 on failure.
 */
int load_ht(hash_table *table, char *file, int type, char *delimeter) {

    if(table->val_printable) {

        // Load from file
        FILE *fp;
        fp = fopen(file, "r");
        char *line = NULL;
        size_t len = 0;
        ssize_t read;

        // Read line by line and add to table
        while((read = getline(&line, &len, fp)) != -1) {

            char *key = strtok(line, delimeter);
            char *_val = strtok(NULL, delimeter);
            int val_len = (int) strlen(_val);
            (_val)[val_len - 1] = '\0'; // remove \n at the end of every line

            if(type == HT_TYPE_INT) {
                int val = atoi(_val);
                size_t size = sizeof(int);
                add_ht(table, key, (void *) &val, size);
            }

            else if(type == HT_TYPE_STR) {
                char val[val_len];
                strcpy(&val[0], _val);
                add_ht(table, key, (void *) &val, val_len);
            }

            else {
                printf("ERROR: Use HT_TYPE_INT (=0) or HT_TYPE_STR (=1)\n");
                exit(1);
                break;
            }
        }

        // Close file
        fclose(fp);
    }


    return 0;
}

/**
 * Prints the table to std out
 * @param table The table to print
 */
void print_ht(hash_table *table) {

    printf("~~~~~~~~~~~~~~~~~~~~~ HASH_TABLE PRINT ~~~~~~~~~~~~~~~~~~~~~\n");

    // For every line in table..
    for (int i = 0; i < table->size; ++i) {
        if((table->data + i)->count > 0) { // i.e. bucket list not empty

            // Print line
            hash_node *first_node_in_bucket_list = (table->data + i)->node;
            char string[first_node_in_bucket_list->val_len];
            (*table->val_print_fn)(&string[0], ((table->data + i)->node)->val);
            printf("%s %s (key: %p, val: %p, node: %p)\n", first_node_in_bucket_list->key, string, first_node_in_bucket_list->key, first_node_in_bucket_list->val, first_node_in_bucket_list);

            // If more than 1 node in bucket_list
            hash_node *traveler;
            if ((table->data + i)->count > 1) {
                traveler = ((table->data + i)->node)->next;
                while(traveler != NULL) {
                    char string[traveler->val_len];
                    (*table->val_print_fn)(&string[0], traveler->val);
                    printf("%s %s \t(key: %p, val: %p, node: %p)\n", traveler->key, string, traveler->key, traveler->val, traveler);
                    traveler = traveler->next;
                }
            }
        }
    }

    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

}


/**
 * Frees the table and its entries.
 * @param  table Pointer to hash_table.
 * @return       0 on success.
 */
int free_ht(hash_table *table) {

    if(DEBUG_HASH_TABLE) printf("---------------------- debug: free hash table ----------------------\n");

    hash_node *tmp;
    hash_node *traveler;

    // Loop through table
    for (int i = 0; i < table->size; ++i) {

        hash_node *traveler = (table->data + i)->node;

        while(traveler != NULL) {

            hash_node *next = traveler->next;

            if(DEBUG_HASH_TABLE) printf("------------- debug: free key(%s) -------------\n", traveler->key);

            if(traveler->key != NULL) {
                if(DEBUG_HASH_TABLE_MEMLEAK) printf("debug: hash_table: memleak: free()ed key %p @ node %p\n", traveler->key, traveler);
                free(traveler->key);
            }
            if(traveler->val != NULL) {
                if(DEBUG_HASH_TABLE_MEMLEAK) printf("debug: hash_table: memleak: free()ed val %p @ node %p\n", traveler->val, traveler);
                free(traveler->val);
            }

            if(DEBUG_HASH_TABLE_MEMLEAK) printf("debug: hash_table: memleak: free()ed node %p\n", traveler);
            free(traveler);

            if(DEBUG_HASH_TABLE) printf("------------------------------------------------\n\n");

            traveler = next;

        }
    }

    // Finally free the rest
    free(table->data);
    if(DEBUG_HASH_TABLE_MEMLEAK) printf("debug: hash_table: memleak: free()ed data @ %p\n", table->data);
    free(table);
    if(DEBUG_HASH_TABLE_MEMLEAK) printf("debug: hash_table: memleak: free()ed table @ %p\n", table);

    if(DEBUG_HASH_TABLE) printf("------------------------------------------------------------------\n\n");

    return 0;
}


/**
 * Hash function by Prof. Daniel J. Bernstein aka djb,
 * the hash function is known as djb2.
 * Source: http://www.cse.yorku.ca/~oz/hash.html
 * @param  table Pointer to hash_table
 * @param  str   Pointer to char, i.e. str to hash
 * @return       returns unsigned long hash value
 */
unsigned long hash(hash_table *table, char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % table->size;
}

/**
 * An example of a print function for the value of type char.
 * @param string         Where the resulting string will be stored
 * @param pointer_to_val Pointer to a pointer to the value
 */
void sprint_val_str(char *string, void **pointer_to_val) {
    sprintf(string, "%s", (char *) pointer_to_val);
}

/**
 * An example of a print function for the value of type int.
 * @param string         Where the resulting string will be stored
 * @param pointer_to_val Pointer to a pointer to the value
 */
void sprint_val_int(char *string, void **pointer_to_val) {
    sprintf(string, "%i", (int) *pointer_to_val);
}

/**
 * Test functions for development 
 */
int test_fn_int() {

    // VALUES OF INTEGERS
    hash_table *h_t;
    if(init_ht(&h_t, 3, HT_VAL_PRINTABLE, sprint_val_int) == 1) {
        // Error, could not init hash table
        exit(1);
    }


    char key[10];

    // Add
    for (int i = 0; i < 12; ++i) {
        sprintf(key, "key%i", i);
        add_ht(h_t, key, (void *)&i, sizeof(int));
    }

    print_ht(h_t);

    // Update
    int new_val;
    for (int i = 0; i < 10; ++i) {
        sprintf(key, "key%i", i);
        new_val = i + 10;
        upd_ht(h_t, key, (void *) &new_val, sizeof(int));
    }

    print_ht(h_t);

    // Remove
    for (int i = 0; i < 5; ++i) {
        sprintf(key, "key%i", i);
        rem_ht(h_t, key);
    }

    // Store hash data
    store_ht(h_t, "./db_int.txt", "-");

    print_ht(h_t);

    free_ht(h_t);

    return 0;
}


int test_fn_str() {

    // VALUES OF STRINGS
    hash_table *h_t;
    if(init_ht(&h_t, 3, HT_VAL_PRINTABLE, sprint_val_str) == 1) {
        // Error, could not init hash table
        exit(1);
    }

    char key[10];
    char val[10];

    // Add
    for (int i = 0; i < 12; ++i) {
        sprintf(key, "key%i", i);
        sprintf(val, "val%i", i);
        add_ht(h_t, key, (void *)&val, strlen(val));
    }

    print_ht(h_t);

    printf("UPDATE NOW:\n");

    // Update
    int new_val;
    for (int i = 7; i < 10; ++i) {
        sprintf(key, "key%i", i);
        new_val = i + 10;
        sprintf(val, "newval %i", new_val);
        upd_ht(h_t, key, (void *) &val, strlen(val));
    }

    printf("END UPDATE\n");

    print_ht(h_t);

    // Remove
    for (int i = 3; i < 5; ++i) {
        sprintf(key, "key%i", i);
        rem_ht(h_t, key);
    }

    print_ht(h_t);

    // Read
    printf("\nRead:\n");
    void *get_val;
    size_t *get_val_len;
    for (int i = 0; i < 12; ++i) {
        sprintf(key, "key%i", i);
        get_ht(h_t, key, &get_val, &get_val_len);

        if(get_val != NULL) {
            printf("read: %s (length: %lu)\n", (char*) get_val, *get_val_len);
        }

    }

    print_ht(h_t);

    // Store hash data
    store_ht(h_t, "./db.txt", "-");

    // Free hash table
    free_ht(h_t);

    return 0;
}

int test_fn_load_str() {

    printf("LOADING HASH TABLE: \n");

    hash_table *h_t_loaded;

    if(init_ht(&h_t_loaded, 256, HT_VAL_PRINTABLE, sprint_val_str) == 1) {
        exit(1);
    }

    load_ht(h_t_loaded, "./db.txt", HT_TYPE_STR, "-");

    print_ht(h_t_loaded);

    free_ht(h_t_loaded);


    return 0;
}

int test_fn_load_int() {

    printf("LOADING HASH TABLE: \n");

    hash_table *h_t_loaded;

    if(init_ht(&h_t_loaded, 3, HT_VAL_PRINTABLE, sprint_val_int) == 1) {
        exit(1);
    }

    load_ht(h_t_loaded, "./db_int.txt", HT_TYPE_INT, "-");

    print_ht(h_t_loaded);

    free_ht(h_t_loaded);


    return 0;
}

// int main(int argc, char const *argv[]) {

//     // test_fn_int();
//     // test_fn_str();
//     // test_fn_load_str();
//     test_fn_load_int();

//     return 0;
// }

