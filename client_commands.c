#include <stdio.h>
#include <stddef.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "client_commands.h"
#include "shell.h"
#include "dns.h"


// Get the most significant byte of a 32-bit int
#define MS_BYTE(n) (((n) & 0xFF000000) >> 3*8)

#define MAX_TOKENS 8

#define MAX_IP_STRLEN 64


static uint8_t packet_buffer[BUFSIZ]; // Buffer for holding packet replies


// Split input string on dots, setting 'tokens' to contain the array
// of token strings. Returns the number of tokens.
static size_t split_on_dots(char* str, char* tokens[], size_t max_tokens);


// Parse an ip in the format "192.3.1.34". Sets the result in 'addr' param.
// Returns true on success and false otherwise.
static bool parse_ip(char* input, uint32_t* addr);


// Render an IPv4 address as a string.
static void ip_as_string(uint32_t addr, char* out);


command_pair dns_lookup_commands[2] = {
  { "lookup", &send_lookup_req },
  { "rev-lookup", &send_rev_lookup_req}
};


// Send a lookup request.
void send_lookup_req(size_t argc, char** argv) {
  char ip_string[MAX_IP_STRLEN]; // To represent reply data

  if (argc != 2) {
    fprintf(stderr, "Usage: lookup domain_name\n");
    return;
  }

  dns_packet* req = new_dns_packet_dom(argv[1], true);

  send_dns_packet(dns_server_fd, req);

  // Get the response
  recv(dns_server_fd, packet_buffer, sizeof(packet_buffer), 0);
  ip_as_string(((dns_packet*)packet_buffer)->contents.ipv4_addr, ip_string);

  // FIXME: validate checksum!!!

  printf("Reply received: IP addr for %s is %s\n", ip_string, argv[1]);

  // Cleanup
  destroy_dns_packet(&req);
}


// Send a reverse lookup request.
void send_rev_lookup_req(size_t argc, char** argv) {
  uint32_t addr;
  char ip_string[MAX_IP_STRLEN]; // For representing input addr

  // FIXME remove!!
  memset(ip_string, 0, sizeof(ip_string));

  if (argc != 2) {
    fprintf(stderr, "Usage: rev-lookup ip\n");
    return;
  }

	// Parse input ip address
  if (!parse_ip(argv[1], &addr)) {
    fprintf(stderr, "send_rev_lookup_req: Malformed IPv4 address!\n");
    return;
  }

  fprintf(stderr, "addr = %u\n", addr);

  dns_packet* req = new_dns_packet_ip(addr, true);

  send_dns_packet(dns_server_fd, req);

  // Get the response
  recv(dns_server_fd, packet_buffer, sizeof(packet_buffer), 0);
  ip_as_string(addr, ip_string);

  // FIXME: validate checksum!!

  printf("Reply received: Domain name for %s is %s\n", ip_string,
    (char*)(packet_buffer + offsetof(dns_packet, contents)));

  // Cleanup
  destroy_dns_packet(&req);
}


static size_t split_on_dots(char* str, char* tokens[], size_t max_tokens) {
  char* token; // Token returned by strtok() 
  size_t token_count = 0; // Number of tokens consumed so far
  char* strarg = str; // Argument to strtok() must be 'str' on first call,
                      // NULL after

  // Consume as many tokens as possible
  while ((token = strtok(strarg, ".")) != NULL) {
    // Setup argument for subsequent calls
    if (token_count == 0) {
      strarg = NULL;
    }

    // Check for space
    if (token_count < max_tokens) {
      tokens[token_count] = token;
    } else {
      // No space left
      break;
    }

    token_count++;
  }

  return token_count;
}


static bool parse_ip(char* input, uint32_t* addr) {
  char* byte_strs[4]; // String representation of 4 bytes in an IPv4 address
  uint32_t bytes[4]; // The 4 bytes making up the address
  unsigned long parsed; // For holding result of an int parse

  if (split_on_dots(input, byte_strs, MAX_TOKENS) != 4) {
    return false;
  }
  
  for (size_t i = 0; i < sizeof(byte_strs)/sizeof(byte_strs[0]); ++i) {
    // Check for valid byte value
    if (((parsed = strtoul(byte_strs[i], NULL, 10)) > 255)) {
      return false;
    }         
    
    bytes[i] = (uint32_t)parsed;

    // FIXME remove!!
    fprintf(stderr, "byte_strs[%lu]: \"%s\"\n", i, byte_strs[i]);
    fprintf(stderr, "byte_strs[%lu]: %u\n", i, bytes[i]);
  }

  *addr = (bytes[0] << 3*8) | (bytes[1] << 2*8) | (bytes[2] << 8) | bytes[3];

  return true;
}


static void ip_as_string(uint32_t addr, char* out) {
  int num_chars;
  char* curr = out;
  for (size_t i = 0; i < 4; ++i, addr <<= 8) {        
    num_chars = sprintf(curr, "%u", MS_BYTE(addr));
    curr += num_chars;

    // FIXME
    fprintf(stderr, "MS_BYTE(addr) = %u\n", MS_BYTE(addr));
    
    if (i < 3) {
      sprintf(curr, ".");
      ++curr;
    }

    assert(num_chars > 0); // XXX: too lazy to do proper error-checking

  }
}