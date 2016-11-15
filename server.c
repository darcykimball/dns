#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "dns.h"
#include "dns_lookup_table.h"

#define DEBUG
#include "debug.h"

#define SERVER_PORT 5432
#define MAX_PENDING 5
#define MAX_LINE 256


// Hard coding a lookup table
static dns_lookup_table* build_table();


// Handle a lookup request
// Returns whether or not the request was able to be handled.
// Params are the buffer holding raw packets, the lookup table to use,
// and the socket to send out the reply to
static bool handle_req(uint8_t const* buf, dns_lookup_table const* table,
  int s, struct sockaddr_in const* dest);


int main() {
  struct sockaddr_in sin; // TODO
  struct sockaddr_in src_addr; // To store address of a sender
  uint8_t buf[MAX_LINE]; // TODO
  socklen_t len; // TODO
  socklen_t src_addr_len;
  int s; // TODO

  dns_lookup_table* table; // The lookup table

  // Build address data structure
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons(SERVER_PORT);

  // TODO
  if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("simplex-talk: socket");
    exit(EXIT_FAILURE);
  }

  if ((bind(s, (struct sockaddr*)&sin, sizeof(sin))) < 0) {
    perror("simplex-talk: bind");
    exit(EXIT_FAILURE);
  }

  // Build the lookup table
  LOG("Building table...");
  table = build_table();
  LOG("done!");

  // Wait for packets, and send out an appropriate reply
  memset(buf, 0, sizeof(buf));

  LOG("Waiting for packets...");
  while ((len = recvfrom(s, buf, sizeof(buf), 0,
                  (struct sockaddr*)&src_addr, &src_addr_len))) { 
    LOG("Got a packet!");

    LOG("Handling request...");
    handle_req(buf, table, s, &src_addr);
    LOG("done!");
  }

  // Cleanup
  destroy_dns_lookup_table(&table);


  return EXIT_SUCCESS;
}


// Hard coding a lookup table
static dns_lookup_table* build_table() {
  dns_lookup_table* table = new_dns_lookup_table();
    
  add_entry(table, "www.google.com", 0x01020304);
  add_entry(table, "www.yahoo.com", 0x05060708);
  add_entry(table, "www.bing.com", 0x090A0B0C);
  add_entry(table, "www.duckduckgo.com", 0xDEADC0DE);

  return table;
}


// Handle a lookup request
// Returns whether or not the request was able to be handled.
static bool handle_req(uint8_t const* buf, dns_lookup_table const* table,
  int s, struct sockaddr_in const* dest) {
  dns_message req_type = AS_DNS_PACKET(buf)->msg;
  entry* found_entry = NULL; // For result of lookup
  dns_packet* reply; // Response to send back

  switch (req_type) {
    case LOOKUP_REQUEST:
      found_entry = lookup_by_name(table,
        (char*)(buf + offsetof(dns_packet, contents)));
      
      if (!found_entry) {
        reply = new_dns_packet_lookup_failed();
      } else {
        reply = new_dns_packet_ip(found_entry->addr, false);
      }

      // Send reply
      send_dns_packet(s, reply, dest);
      
      break;

    case REVERSE_LOOKUP_REQUEST:
      found_entry = lookup_by_addr(table,
        AS_DNS_PACKET(buf)->contents.ipv4_addr);

      if (!found_entry) {
        reply = new_dns_packet_lookup_failed();
      } else {
        LOG("Found entry:");
        LOG(found_entry->name);
        reply = new_dns_packet_dom(found_entry->name, false);
      }

      // Send reply
      send_dns_packet(s, reply, dest);

      break;

    default:
      return false;
  }

  // Cleanup
  destroy_dns_packet(&reply);

  return true;
}
