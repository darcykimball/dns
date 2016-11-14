#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

#include "lookup_table.h"
#include "hash_table.h"
#include "SuperFastHash.h"


// Construct an empty lookup table
lookup_table* new_lookup_table() {
  return new_hash_table(&hash_entry);
}


void add_entry(lookup_table* table, char const* name, uint32_t addr) {
  entry* e = (entry*)malloc(sizeof(entry));
  e->name = name;
  e->addr = addr;

  add_hash_table(table, e);
}


entry* lookup(lookup_table* table, char const* key) {
  entry e = { key, 0 }; // Dummy for searching
  return (entry*)find_hash_table(table, &e, &name_eq);
}


// TODO!!
void destroy_lookup_table(lookup_table** ltpp);


bool name_eq(void const* e1, void const* e2) {
  return strcmp(AS_ENTRY(e1)->name, AS_ENTRY(e2)->name) == 0;
}


size_t hash_entry(void const* e) {
  return SuperFastHash(AS_ENTRY(e)->name, strlen(AS_ENTRY(e)->name));
}
