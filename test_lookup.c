#include <stdio.h>
#include <string.h>

#include "dns_lookup_table.h"
#include "test.h"


int main() {
  dns_lookup_table* table = new_dns_lookup_table();

  add_entry(table, "www.google.com", 0x01020304);
  add_entry(table, "www.yahoo.com", 0x05060708);
  add_entry(table, "www.bing.com", 0x090A0B0C);
  add_entry(table, "www.duckduckgo.com", 0xDEADC0DE);


  TEST_ASSERT(lookup_by_name(table, "www.google.com") != NULL);
  TEST_ASSERT(lookup_by_name(table, "www.yahoo.com") != NULL);
  TEST_ASSERT(lookup_by_name(table, "www.bing.com") != NULL);
  TEST_ASSERT(lookup_by_name(table, "www.duckduckgo.com") != NULL);


  TEST_ASSERT(lookup_by_name(table, "www.google.com")->addr == 0x01020304);
  TEST_ASSERT(lookup_by_name(table, "www.yahoo.com")->addr == 0x05060708);
  TEST_ASSERT(lookup_by_name(table, "www.bing.com")->addr == 0x090A0B0C);
  TEST_ASSERT(lookup_by_name(table, "www.duckduckgo.com")->addr == 0xDEADC0DE);
  

  TEST_ASSERT(strcmp(lookup_by_addr(table, 0x01020304)->name, "www.google.com") == 0);
  TEST_ASSERT(strcmp(lookup_by_addr(table, 0x05060708)->name, "www.google.com") == 0);
  TEST_ASSERT(strcmp(lookup_by_addr(table, 0x090A0B0C)->name, "www.google.com") == 0);
  TEST_ASSERT(strcmp(lookup_by_addr(table, 0xDEADC0DE)->name, "www.google.com") == 0);


  destroy_dns_lookup_table(&table);

  TEST_ASSERT(table == NULL);

}
