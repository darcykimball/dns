#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "dns.h"
#include "test.h"

#define DEBUG
#include "debug.h"


dns_packet* new_dns_packet_dom(char const* domain_name, bool is_request) {
  dns_packet* packet; // The new packet
  size_t domain_name_len = strlen(domain_name) + 1; // Length of payload
  // ^ XXX: Why are we sending the null byte? For this particular application,
  // the receiver would probably need to either set a byte in the receive
  // buffer to the null byte (to even use the payload as a string), potentially
  // clobbering other data, or else copy the whole thing.
  // Both are unnecessarily wasteful. Keeping the null byte lets the receiver
  // use the string part of the raw data directly.
  // Also, not doing this led to some bugs: I forgot to consistently account
  // for the lack of the null byte, leading to all sorts of mysterious data.

  packet = (dns_packet*)malloc(sizeof(dns_packet));

  // Determine message type
  if (is_request) {
    packet->msg = LOOKUP_REQUEST;
  } else { // This is a response
    packet->msg = REVERSE_LOOKUP_RESPONSE;
  }

  packet->contents.domain_name = domain_name;
  packet->len = domain_name_len;
  packet->checksum = checksum(packet);

  return packet;
}


dns_packet* new_dns_packet_ip(uint32_t ipv4_addr, bool is_request) {
  dns_packet* packet; // The new packet

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
  packet->checksum = checksum(packet);

  return packet;
}


dns_packet* new_dns_packet_lookup_failed(bool is_lookup) {
  dns_packet* packet; // The new packet

  packet = (dns_packet*)malloc(sizeof(dns_packet));
  packet->msg = is_lookup ? LOOKUP_RESPONSE : REVERSE_LOOKUP_RESPONSE;
  packet->len = 0;
  packet->contents.ipv4_addr = 0;
  packet->checksum = checksum(packet);

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

  // FIXME: remove!!
  fprintf(stderr, "send_dns_packet():\n");
  DUMPI(packet->msg);
  DUMPI(packet->len);
  DUMPI(packet->checksum);

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
    curr->iov_base = (char*)(packet->contents.domain_name);
    curr++->iov_len = packet->len;
  } else {
    curr->iov_base = &packet->contents.ipv4_addr;
    curr++->iov_len = sizeof(packet->contents.ipv4_addr);
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


void deserialize(dns_packet* received, uint8_t const* buf) {
  // Msg type
  received->msg = *(dns_message*)buf;
  buf += sizeof(dns_message);

  // Length
  received->len = *(uint32_t*)buf;
  buf += sizeof(uint32_t);

  switch (received->msg) {
    case LOOKUP_REQUEST:
      received->contents.domain_name = (char const*)buf;
      break;
    case REVERSE_LOOKUP_RESPONSE:
      received->contents.domain_name = (char const*)buf;
      break;
    case LOOKUP_RESPONSE:
      received->contents.ipv4_addr = *(uint32_t*)buf;
      break;
    case REVERSE_LOOKUP_REQUEST:
      received->contents.ipv4_addr = *(uint32_t*)buf;
      break;
    default:
      // FIXME
      return;
  }

  // Special case: this is potentially a 'lookup failed' packet.
  // Account for the 4 bytes of nothing where 'contents' should be.
  if (received->len == 0) {
    buf += sizeof(uint32_t);
  }
  buf += received->len;

  received->checksum = *(uint32_t*)buf;
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
      packet->len == (strlen(packet->contents.domain_name) + 1);
  } else if (
    packet->msg == LOOKUP_RESPONSE || packet->msg == REVERSE_LOOKUP_REQUEST) {
    return packet->len == sizeof(packet->contents.ipv4_addr);
  }

  // The msg field is malformed
  return false;
}


bool is_lookup_failed_packet(dns_packet const* packet) {
  return packet->len == 0;
}

  
uint32_t checksum(dns_packet const* packet) {
  // XXX: We have to do this calculation because when sending a packet, it's
  // not simply a serialized struct! It's field-by-field! Lesson (hopefully)
  // learned.
  size_t other_fields_size =
      sizeof(dns_message) // message type
    + 2*sizeof(uint32_t); // len and checksum

  // Special case: packet is potentially 'lookup failed' packet
  if (packet->len == 0) {
    return other_fields_size + sizeof(uint32_t);
  }

  // Count the total number of bytes in the packet
  if (has_string_payload(packet)) {
    return other_fields_size + packet->len;
  } else {
    return other_fields_size + sizeof(uint32_t);
  }
}


void destroy_dns_packet(dns_packet** dppp) {
  free(*dppp);
  *dppp = NULL;
}
