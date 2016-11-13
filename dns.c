#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "dns.h"
#include "test.h"


// Serialize a dns packet into a heap-alloc'd buffer
static void* serialize_dns_packet(dns_packet const* packet);


dns_packet* new_dns_packet_dom(char const* domain_name, bool isRequest) {
  dns_packet* packet; // The new packet
  size_t domain_name_len = strlen(domain_name); // Length of payload

  packet = (dns_packet*)malloc(sizeof(packet));

  // Determine message type
  if (isRequest) {
    packet->msg = LOOKUP_REQUEST;
  } else { // This is a response
    packet->msg = REVERSE_LOOKUP_RESPONSE;
  }

  packet->contents.domain_name = domain_name;
  packet->len = sizeof(packet) - sizeof(packet->contents)
                  + domain_name_len;
  packet->checksum = checksum((uint8_t*)domain_name, domain_name_len);

  return packet;
}


dns_packet* new_dns_packet_ip(uint32_t ipv4_addr, bool isRequest) {
  dns_packet* packet; // The new packet

  // ip addr in network order
  uint32_t network_order_ip = htonl(ipv4_addr);

  packet = (dns_packet*)malloc(sizeof(packet));

  // Determine message type
  if (isRequest) {
    packet->msg = REVERSE_LOOKUP_REQUEST;
  } else { // This is a response
    packet->msg = LOOKUP_RESPONSE;
  }

  packet->contents.ipv4_addr = ipv4_addr;
  packet->len = sizeof(packet);
  packet->checksum = checksum((uint8_t*)&network_order_ip, sizeof(ipv4_addr));

  return packet;
}


int send_dns_packet(int sockfd, dns_packet const* packet) {
  struct msghdr header; // Message header for sendmsg()
  struct iovec iov; // iovec for data to send
  void* data; // Raw data to send

  // Validate packet
  if (!is_valid_dns_packet(packet)) {
    // Malformed packet
    return -1;
  }

  // Allocate for and initialize raw data
  data = serialize_dns_packet(packet);

  // Initialize sendmsg() params
  memset(&header, 0, sizeof(header));

  iov.iov_base = data;
  iov.iov_len = packet->len;

  header.msg_iov = &iov;
  header.msg_iovlen = 1; // We have a single gather location

  return sendmsg(sockfd, &header, 0);
}


bool is_valid_dns_packet(dns_packet const* packet) {
  if (
    packet->msg == LOOKUP_REQUEST || packet->msg == REVERSE_LOOKUP_RESPONSE
  ) {
    return
      packet->len == sizeof(packet) - sizeof(packet->contents)
                       + strlen(packet->contents.domain_name);
  } else if (
    packet->msg == LOOKUP_RESPONSE || packet->msg == REVERSE_LOOKUP_REQUEST) {
    return packet->len == sizeof(packet);
  }

  // The msg field is malformed
  return false;
}


static void* serialize_dns_packet(dns_packet const* packet) {
  void* data; // Data buffer
  uint8_t* curr = 0; // Pointer to current position in data
  size_t domain_name_len = sizeof(packet) - sizeof(packet->msg) -
    sizeof(packet->len) - sizeof(packet->checksum); // Len of string, if any

  data = malloc(sizeof(packet));

  // Copy the data
  memcpy(curr, &packet->msg, sizeof(packet->msg));
  curr += sizeof(packet->msg);
  memcpy(curr, &packet->len, sizeof(packet->len));
  curr += sizeof(packet->len);

  if (has_string_payload(packet)) {
    memcpy(curr, packet->contents.domain_name, domain_name_len); 
    curr += sizeof(domain_name_len);
  } else {
    memcpy(curr, &packet->contents.ipv4_addr,
      sizeof(packet->contents.ipv4_addr));
    curr += sizeof(packet->contents.ipv4_addr);
  }

  memcpy(curr, &packet->checksum, sizeof(packet->checksum));

  return data;
}
