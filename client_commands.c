#include <stdio.h>
#include <stddef.h>
#include <stddef.h>
#include <string.h>

#include "client_commands.h"
#include "shell.h"
#include "dns.h"


#define MAX_TOKENS 8


// Split input string on dots, setting 'tokens' to contain the array
// of token strings. Returns the number of tokens.
static size_t split_on_dots(char* str, char* tokens[], size_t max_tokens);


// Parse an ip in the format "192.3.1.34". Sets the result in 'addr' param.
// Returns true on success and false otherwise.
static bool parse_ip(char* input, uint32_t* addr);


command_pair dns_lookup_commands[2] = {
  { "lookup", &send_lookup_req },
  { "rev-lookup", &send_rev_lookup_req}
};


// Send a lookup request.
void send_lookup_req(size_t argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: lookup domain_name");
    return;
  }

  dns_packet* req = new_dns_packet_dom(argv[1], true);

  send_dns_packet(dns_server_fd, req);
}


// Send a reverse lookup request.
void send_rev_lookup_req(size_t argc, char** argv) {
  uint32_t addr;

  if (argc != 2) {
    fprintf(stderr, "Usage: rev-lookup ip\n");
    return;
  }

	// Parse input ip address
  if (!parse_ip(argv[1], &addr)) {
    fprintf(stderr, "send_rev_lookup_req: Malformed IPv4 address!\n");
    return;
  }

  dns_packet* req = new_dns_packet_ip(addr, true);

  send_dns_packet(dns_server_fd, req);
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
  uint8_t bytes[4]; // The 4 bytes making up the address
  unsigned long parsed; // For holding result of an int parse

  if (split_on_dots(input, byte_strs, MAX_TOKENS) != 4) {
    return false;
  }
  
  for (size_t i = 0; i < sizeof(byte_strs)/sizeof(byte_strs[0]); ++i) {
    // Check for valid byte value
    if ((parsed = strtoul(byte_strs[i], NULL, 10) > 255)) {
      return false;
    }         

    bytes[i] = (uint8_t)parsed;
  }

  *addr = (bytes[0] << 3) & (bytes[1] << 2) & (bytes[2] << 1) & bytes[0];

  return true;
}
