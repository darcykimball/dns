#ifndef DNS_LOOKUP_TABLE_H
#define DNS_LOOKUP_TABLE_H


#include <inttypes.h>

#include "bimap.h"


// A lookup table entry; keys are strings (domain names), and values are IP
// addresses
typedef struct {
  char const* name;
  uint32_t addr;
} entry;


#define AS_ENTRY(vp) ((entry*)(vp))


// A DNS lookup table
typedef bimap dns_lookup_table;


// Construct an empty lookup table
dns_lookup_table* new_dns_lookup_table();


// Add an entry to a lookup table
void add_entry(dns_lookup_table* table, char const* name, uint32_t addr);


// Lookup an entry in the table
entry* lookup_name(dns_lookup_table* table, char const* name);


// Reverse-lookup an entry in the table
entry* lookup_addr(dns_lookup_table* table, uint32_t addr);


// Destroy a lookup table
void destroy_dns_lookup_table(dns_lookup_table** ltpp);


// Equivalence on entries; by domain names
bool name_eq(void const* name1, void const* name2);


// Equivalence on entries; by ip addresses
bool addr_eq(void const* addr1, void const* addr2);


#endif // DNS_LOOKUP_TABLE_H
