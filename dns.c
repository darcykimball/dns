#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "dns.h"
#include "test.h"


dns_packet* new_dns_packet_dom(char const* domain_name, bool is_request) {
  dns_packet* packet; // The new packet
  size_t domain_name_len = strlen(domain_name); // Length of payload

  packet = (dns_packet*)malloc(sizeof(dns_packet));

  // Determine message type
  if (is_request) {
    packet->msg = LOOKUP_REQUEST;
  } else { // This is a response
    packet->msg = REVERSE_LOOKUP_RESPONSE;
  }

  packet->contents.domain_name = domain_name;
  packet->len = domain_name_len;
  packet->checksum = checksum((uint8_t*)domain_name, domain_name_len);

  return packet;
}


dns_packet* new_dns_packet_ip(uint32_t ipv4_addr, bool is_request) {
  dns_packet* packet; // The new packet

  // ip addr in network order
  uint32_t network_order_ip = htonl(ipv4_addr);

  packet = (dns_packet*)malloc(sizeof(dns_packet));

  // Determine message type
  if (is_request) {
    packet->msg = REVERSE_LOOKUP_REQUEST;
  } else { // This is a response
    packet->msg = LOOKUP_RESPONSE;
  }

  // FIXME: do we need to convert this address to network order??
  packet->contents.ipv4_addr = ipv4_addr;
  packet->len = sizeof(packet->contents.ipv4_addr);
  packet->checksum = checksum((uint8_t*)&network_order_ip, sizeof(ipv4_addr));

  return packet;
}


dns_packet* new_dns_packet_lookup_failed() {
  dns_packet* packet; // The new packet

  packet = (dns_packet*)malloc(sizeof(dns_packet));
  memset(packet, 0, sizeof(dns_packet));  

  return packet;
}


int send_dns_packet(int sockfd, dns_packet* packet, struct sockaddr_in const* dest) {
  struct msghdr header; // Message header for sendmsg()
  struct iovec* iov; // iovec for data to send
  struct iovec* curr; // For traversing the iovec

  // Validate packet
  if (!is_valid_dns_packet(packet)) {
    // Malformed packet
    return -1;
  }

  // Initialize sendmsg() params
  memset(&header, 0, sizeof(header));

  // Build iovec
  iov = (struct iovec*)malloc(DNS_PACKET_N_FIELDS * sizeof(struct iovec));
  curr = iov;

  curr->iov_base = &packet->msg;
  curr++->iov_len = sizeof(packet->msg);

  curr->iov_base = &packet->len;
  curr++->iov_len = sizeof(packet->len);

  if (has_string_payload(packet)) {
    // XXX: the cast is here to silence compiler warnings regarding const-ness.
    // we're using void* already so full const-correctness is already screwed
    // anyway.
    curr->iov_base = (char*)packet->contents.domain_name;
    curr++->iov_len = packet->len;
  } else {
    curr->iov_base = &packet->contents;
    curr++->iov_len = sizeof(packet->contents);
  }

  curr->iov_base = &packet->checksum;
  curr->iov_len = sizeof(packet->checksum);

  // Address the packet if needed
  if (dest != NULL) {
    header.msg_name = (void*)dest;  
    header.msg_namelen = sizeof(struct sockaddr_in);
  }

  header.msg_iov = iov;
  header.msg_iovlen = DNS_PACKET_N_FIELDS; 

  return sendmsg(sockfd, &header, 0);
}


bool is_valid_dns_packet(dns_packet const* packet) {
  // Special case: packet is 'lookup failed'
  if (is_lookup_failed_packet(packet)) {
    return true;
  }

  if (
    packet->msg == LOOKUP_REQUEST || packet->msg == REVERSE_LOOKUP_RESPONSE
  ) {
    return
      packet->len == strlen(packet->contents.domain_name);
  } else if (
    packet->msg == LOOKUP_RESPONSE || packet->msg == REVERSE_LOOKUP_REQUEST) {
    return packet->len == sizeof(packet->contents.ipv4_addr);
  }

  // The msg field is malformed
  return false;
}


bool is_lookup_failed_packet(dns_packet const* packet) {
  uint8_t* curr_byte = (uint8_t*)packet;
  for (size_t i = 0; i < sizeof(dns_packet); ++i) {
    if (*curr_byte != 0) {
      return false;
    }
  }

  return true;
}


uint32_t checksum(uint8_t const* payload, size_t n) {
  uint32_t sum = 0;
  for (size_t i = 0; i < n; ++i) {
    sum += *payload++;
  }

  return sum;
}


void destroy_dns_packet(dns_packet** dppp) {
  free(*dppp);
  *dppp = NULL;
}
