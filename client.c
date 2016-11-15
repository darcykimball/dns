#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "dns.h"
#include "debug.h"

#define SERVER_PORT 5432 
#define MAX_LINE 256

int main(int argc, char** argv) {
  FILE* fp; // TODO
  struct hostent* hp; // TODO
  struct sockaddr_in sin; // TODO
  char* host; // TODO
  char buf[MAX_LINE]; // TODO
  int s; // TODO
  int len; // TODO

  // Get hostname if provided
  if (argc == 2) {
    host = argv[1];
  } else {
    fprintf(stderr, "usage: %s host\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // Translate host name into peer's IP address
  hp = gethostbyname(host);
  DUMPS(hp->h_name);
  DUMPS(hp->h_addr);

  if (!hp) {
    fprintf(stderr, "dns-client: unknown host: %s\n", host);
    exit(EXIT_FAILURE);
  }

  // Build address data structure
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  memcpy(&sin.sin_addr, hp->h_addr, hp->h_length);
  sin.sin_port = htons(SERVER_PORT);

  // 
  if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("dns-client: socket");
    exit(EXIT_FAILURE);
  }

  // Main loop; get input and send some dns packets
  //printf("Enter packets in form: [rev] domain_name\n");
  //printf("dns>\n");
  //while (fgets(buf, sizeof(buf), stdin)) {
  //  buf[MAX_LINE - 1] = '\0'; // Null-terminate buffer
  //  len = strlen(buf) + 1;

  //  // Send the message
  //  send(s, buf, len, 0);
  //  
  //  printf("dns>\n");
  //}

  // Small test packet
  dns_packet* test_packet = new_dns_packet_dom("www.somesite.com", true);

  send_dns_packet(s, test_packet);

  return EXIT_SUCCESS;
}