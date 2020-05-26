#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// DEFINITIONS

// One can use this defs instead of 0 or 1 to init_ht()
#define HT_TYPE_INT 0
#define HT_TYPE_STR 1
#define HT_VAL_NOT_PRINTABLE 0
#define HT_VAL_PRINTABLE 1

// Debug
#define DEBUG_HASH_TABLE 0
#define DEBUG_HASH_TABLE_MEMLEAK 0


// STRUCTS

// Used to store a value inside a bucket_list
struct hash_node {
    char *key;
    void **val;
    size_t val_len;
    struct hash_node *next;
};
typedef struct hash_node hash_node;

// The hash table contains size * bucket lists
struct bucket_list {
    hash_node *node;
    int count;
};
typedef struct bucket_list bucket_list;


// Finally the hash_table.
// data will be malloc()ed, in count we'll keep track of the entries
// and size will be set for the size of the whole map.
// Collisions tracks the number of collisions
struct hash_table {
    bucket_list *data;
    int count;
    int size;
    int collisions;
    int val_printable;
    void (*val_print_fn)();
};
typedef struct hash_table hash_table;


// PROTOTYPES
int init_ht(hash_table **table, int size, int val_printable, void (*val_print_fn)());
hash_node *get_el_in_bucket_list(hash_table *table, char *key);
int add_ht(hash_table *table, char *key, void **val, size_t val_len);
int upd_ht(hash_table *table, char *key, void **val, size_t val_len);
int rem_ht(hash_table *table, char *key);
int get_ht(hash_table *table, char *key, void **val, size_t **val_len);
int store_ht(hash_table *table, char *file, char *delimeter);
int load_ht(hash_table *table, char *file, int type, char *delimeter);
void print_ht(hash_table *table);
int free_ht(hash_table *table);
unsigned long hash(hash_table *table, char *str);
void sprint_val_str(char *string, void **pointer_to_val);
void sprint_val_int(char *string, void **pointer_to_val);

// Test fns 
int test_fn_int();
int test_fn_str();
int test_fn_load_str();
int test_fn_load_int();

//------------------------------------------------------------
