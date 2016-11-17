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
    if (!handle_req(buf, table, s, &src_addr)) {
      fprintf(stderr, "Couldn't handle request!\n");
    }
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
  add_entry(table, "www.gmail.com", 0xDEADC0DE);
  add_entry(table, "www.drive.google.com", 0xC0DEC0DE);
  add_entry(table, "www.youtube.com", 0x0C0B0A09);
  add_entry(table, "www.twitter.com", 0xDEADBEEF);
  add_entry(table, "www.tumblr.com", 0xCAFEBABE);
  add_entry(table, "www.blogspot.com", 0x08070605);
  add_entry(table, "www.facebook.com", 0x04030201);

  return table;
}


// Handle a lookup request
// Returns whether or not the request was able to be handled.
static bool handle_req(uint8_t const* buf, dns_lookup_table const* table,
  int s, struct sockaddr_in const* dest) {
  entry* found_entry = NULL; // For result of lookup
  dns_packet* reply; // Response to send back

  dns_packet req; // To store received request
  deserialize(&req, buf);

  // Check checksum
  if (checksum(&req) != req.checksum) {
    LOG("Checksum check failed!");
    return false;
  }

  // Handle based on type of message
  switch (req.msg) {
    case LOOKUP_REQUEST:
      // Lookup
      found_entry = lookup_by_name(table, req.contents.domain_name);
      
      if (!found_entry) {
        reply = new_dns_packet_lookup_failed(true);
      } else {
        LOG("Found domain name entry");
        reply = new_dns_packet_ip(found_entry->addr, false);
      }
      
      break;

    case REVERSE_LOOKUP_REQUEST:
      // Reverse lookup
      found_entry = lookup_by_addr(table, req.contents.ipv4_addr);

      if (!found_entry) {
        reply = new_dns_packet_lookup_failed(false);
      } else {
        LOG("Found IP entry:");
        LOG(found_entry->name);
        reply = new_dns_packet_dom(found_entry->name, false);
      }

      break;

    default:
      // Invalid message type
      return false;
  }


  // Send reply
  if ((send_dns_packet(s, reply, dest) < 0)) {
    fprintf(stderr, "handle_req(): Unable to send reply!\n");
    return false;
  }

  // Cleanup
  destroy_dns_packet(&reply);

  return true;
}
