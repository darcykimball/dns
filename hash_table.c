#include "hash_table.h"
#include "list.h"


// Translate a hash to an index, naively
static size_t hashToIndex(size_t hash);


// Construct an empty table
hash_table* new_hash_table(hasher hash_fn) {
  hash_table* table = (hash_table*)malloc(sizeof(hash_table));

  table->hash_fn = hash_fn;
  for (size_t i = 0; i < N_BUCKETS; ++i) {
    table->buckets[i] = nil();
  }

  return table;
}


// Add an element
void add_hash_table(hash_table* table, void* elem) {
  size_t bucket_index = hashToIndex(table->hash_fn(elem));
  list* bucket = table->buckets[bucket_index];

  cons(elem, bucket);
}


// Find an element
// Returns NULL if not found
void* find_hash_table(hash_table* table, void* elem, eq_rel eq_fn) {
  size_t bucket_index = hashToIndex(table->hash_fn(elem));
  list* bucket = table->buckets[bucket_index];
  
  // XXX: seems trivial, but check this is safe??
  if (bucket->count == 1) {
    return bucket->head->data;
  }

  return find_list(bucket, elem, eq_fn);
}


static size_t hashToIndex(size_t hash) {
  return hash % N_BUCKETS;
}
