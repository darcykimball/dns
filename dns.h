#ifndef DNS_H
#define DNS_H

#include <stddef.h>
#include <inttypes.h>

// Types of messages
typedef enum {
  LOOKUP_REQUEST,
  LOOKUP_RESPONSE,
  REVERSE_LOOKUP_REQUEST,
  REVERSE_LOOKUP_RESPONSE
} dns_message;

// Type of payloads; either a string (some domain name) or an IP address (a
// 32-bit word)
typedef union {
  char const* domain_name;
  uint32_t ipv4_addr;
} payload;

// Packet format
typedef struct {
  dns_message msg;
  uint32_t len;
  payload contents;
  uint32_t checksum; 
} dns_packet;


// Construct a string-payload dns packet
dns_packet* new_dns_packet_dom(char const* domain_name, bool isRequest);


// Construct a ip-addr-payload dns packet
dns_packet* new_dns_packet_ip(uint32_t ipv4_addr, bool isRequest);


// Send a dns packet as a message (datagram)
// Returns size of packet sent, otherwise -1 for error
int send_dns_packet(int sockfd, dns_packet const*);


// Validate a dns packet
bool is_valid_dns_packet(dns_packet const*);


// Compute the checksum of a payload
uint32_t checksum(uint8_t* payload, size_t n);  

// Find out if the packet contains a string payload (var. length) or not
// (fixed length), assuming it's well-formed.
#define has_string_payload(p) \
  ((p)->msg == LOOKUP_REQUEST || (p)->msg == REVERSE_LOOKUP_RESPONSE)


#endif // DNS_H
