# Documentation `hash_table`

## Usage

Include the provided `hash_table.h` file:

```c
#include "hash_table/hash_table.h"
```

To initiate a hash table of size 997, use `init_ht()`:

```c
hash_table *my_table;

if(init_ht(&my_table, 997, NULL, NULL) == 1) exit(1);

```

If initialization fails, the function will return a 1. 

Now you can add, update or remove elements like this:

```c
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
```

<br>
**Important**:
You should free the table at the end, to `free()` the `malloc()ed` space, like this:

```c
free_ht(my_table);
```

There are more options documented below.


## Functions
```c
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
int init_ht(hash_table **table, int size, int val_printable, void (*val_print_fn)());
```


```c
/**
 * Given a table and a key, this function will return a pointer to a node
 * with the same key in the hash table.
 * @param  table Pointer to table
 * @param  key   The key
 * @return       Pointer to node with the same key or NULL if it cant be found.
 */
hash_node *get_el_in_bucket_list(hash_table *table, char *key);
```


```c
/**
 * Add an (key,val) pair to the hash table.
 * @param   table     Pointer to a hash_table
 * @param   key       The key
 * @param   value     Pointer to a pointer of value.
 * @param   val_len   The size of the value.
 * @return            0 on success, 1 on failure.
 */
int add_ht(hash_table *table, char *key, void **val, size_t val_len);
```


```c
/**
 * Update an item in the hash_table
 * @param  table    Pointer to table
 * @param  key      The key for which the value will be updated
 * @param  value    Pointer to a pointer of a new value.
 * @param  val_len  The size of the new value.
 * @return          0 on success, 1 on failure.
 */
int upd_ht(hash_table *table, char *key, void **val, size_t val_len);
```


```c
/**
 * Removes the node with for the given key
 * @param  table Pointer to the hash table
 * @param  key   Key
 * @return       0 on succcess, 1 on failure.
 */
int rem_ht(hash_table *table, char *key);
```


```c
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
int get_ht(hash_table *table, char *key, void **val, size_t **val_len);
```


```c
/**
 * This function will store the contents of a hash_table to a file.
 * @param   table       The hash_table to store
 * @param   file        The file to write the contents into.
 * @param   delimeter   The delimeter between key and value which is going 
 *                      to be used in the text file.
 * @return              0 on success, 1 on failure.
 */
int store_ht(hash_table *table, char *file, char *delimeter);
```


```c
/**
 * Loads the contentes of a file to the hash_table table
 * @param  table       The hash_table to load the content into
 * @param  file        The file from where the content is taken
 * @param  type        The type to store. Currently either int: HT_TYPE_INT = 0 or str: HT_TYPE_STR = 1
 * @param  delimeter   The delimeter between key and value which is going 
 *                     to be used in the text file.
 * @return             0 on success, 1 on failure.
 */
int load_ht(hash_table *table, char *file, int type, char *delimeter);
```


```c
/**
 * Prints the table to std out
 * @param table The table to print
 */
void print_ht(hash_table *table);
```


```c
/**
 * Frees the table and its entries.
 * @param  table Pointer to hash_table.
 * @return       0 on success.
 */
int free_ht(hash_table *table);
```


```c
/**
 * Hash function by Prof. Daniel J. Bernstein aka djb,
 * the hash function is known as djb2.
 * Source: http://www.cse.yorku.ca/~oz/hash.html
 * @param  table Pointer to hash_table
 * @param  str   Pointer to char, i.e. str to hash
 * @return       returns unsigned long hash value
 */
unsigned long hash(hash_table *table, char *str);
```


### Examples of print functions

What you basically get is a pointer to a string and the value you stored. Cast and dereference this it and store inside the string your information. 


```c
/**
 * An example of a print function for the value of type char.
 * @param string         Where the resulting string will be stored
 * @param pointer_to_val Pointer to a pointer to the value
 */
void sprint_val_str(char *string, void **pointer_to_val) {
    sprintf(string, "%s", (char *) pointer_to_val);
}
```


```c
/**
 * An example of a print function for the value of type int.
 * @param string         Where the resulting string will be stored
 * @param pointer_to_val Pointer to a pointer to the value
 */
void sprint_val_int(char *string, void **pointer_to_val) {
    sprintf(string, "%i", (int) *pointer_to_val);
}
```