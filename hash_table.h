#ifndef HASH_TABLE_H
#define HASH_TABLE_H


#include "list.h"


#define N_BUCKETS 256


// The type of hashing functions
typedef size_t (*hasher)(void const*);


// A separate-chained hash table, no notion of equivalence on elements
typedef struct {  
  list* buckets[N_BUCKETS]; // XXX: Yes, everything is on the heap
  hasher hash_fn;
} hash_table;


// Construct an empty table
hash_table* new_hash_table(hasher hash_fn);


// Add an element
void add_hash_table(hash_table* table, void* elem);


// Find an element
// Returns NULL if not found
void* find_hash_table(hash_table* table, void* elem, eq_rel eq_fn);


// Destructor
// TODO
void destroy_hash_table(hash_table** tpp);


#endif // HASH_TABLE_H
