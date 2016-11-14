#ifndef LOOKUP_TABLE_H
#define LOOKUP_TABLE_H


#include <inttypes.h>

#include "hash_table.h"


// A lookup table entry; keys are strings (domain names), and values are IP
// addresses
typedef struct {
  char const* name;
  uint32_t addr;
} entry;


#define AS_ENTRY(vp) ((entry*)(vp))


// A DNS lookup table
typedef hash_table lookup_table;


// Construct an empty lookup table
lookup_table* new_lookup_table();


// Add an entry to a lookup table
void add_entry(lookup_table* table, char const* name, uint32_t addr);


// Lookup an entry in the table
entry* lookup(lookup_table* table, char const* key);


// Destroy a lookup table
void destroy_lookup_table(lookup_table** ltpp);


// Equivalence on entries; by domain names
bool name_eq(void const* e1, void const* e2);


// Hash function on entries
size_t hash_entry(void const* e);


#endif // LOOKUP_TABLE_H
