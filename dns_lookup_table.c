#include <inttypes.h>
#include <string.h>

#include "dns_lookup_table.h"
#include "bimap.h"


dns_lookup_table* new_dns_lookup_table() {
  return new_bimap(&name_eq, &addr_eq);
}


void add_entry(dns_lookup_table* table, char const* name, uint32_t addr) {
  entry* e = (entry*)malloc(sizeof(entry)); 
  e->name = name;
  e->addr = addr;

  insert_bimap(table, e);
}


entry* lookup_by_name(dns_lookup_table const* table, char const* name) {
  entry dummy = { name, 0 };
  return AS_ENTRY(find_by_fst(table, &dummy));
}


entry* lookup_by_addr(dns_lookup_table const* table, uint32_t addr) {
  entry dummy = { NULL, addr };
  return AS_ENTRY(find_by_snd(table, &dummy));
}


void destroy_dns_lookup_table(dns_lookup_table** ltpp) {
  destroy_bimap(ltpp);
}


bool name_eq(void const* name1, void const* name2) {
  return strcmp((char const*)name1, (char const*)name2) == 0;
}


bool addr_eq(void const* addr1, void const* addr2) {
  return *(uint32_t*)addr1 == *(uint32_t*)addr2;
}
