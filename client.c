#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "debug.h"
#include "shell.h"
#include "client_commands.h"

#define SERVER_PORT 5432
#define MAX_LINE 256

// For use with commands
int dns_server_fd;

int main(int argc, char** argv) {
  struct hostent* hp; // TODO
  struct sockaddr_in sin; // TODO
  struct sockaddr_in sin_client; // TODO
  char* host; // TODO
  char buf[MAX_LINE]; // TODO
  int s; // TODO
  int len; // TODO

  // Get hostname if provided
  if (argc == 2) {
    host = argv[1];
  } else {
    fprintf(stderr, "usage: simplex-talk host\n");
    exit(EXIT_FAILURE);
  }

  // Translate host name into peer's IP address
  hp = gethostbyname(host);
  DUMPS(hp->h_name);
  DUMPS(hp->h_addr);

  if (!hp) {
    fprintf(stderr, "simplex-talk: unknown host: %s\n", host);
    exit(EXIT_FAILURE);
  }

  // Build address data structure
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  memcpy(&sin.sin_addr, hp->h_addr, hp->h_length);
  sin.sin_port = htons(SERVER_PORT);

  if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("simplex-talk: socket");
    exit(EXIT_FAILURE);
  }

  // Set socket file descriptor for commands
  dns_server_fd = s;

  // Build address data structure for client
  memset(&sin_client, 0, sizeof(sin_client));
  sin_client.sin_family = AF_INET;
  sin_client.sin_addr.s_addr = INADDR_ANY;
  sin_client.sin_port = htons(0); // Let OS give us some port

  if ((bind(s, (struct sockaddr*)&sin_client, sizeof(sin_client)) < 0)) {
    perror("simplex-talk: bind");
    exit(EXIT_FAILURE);
  }

  if (connect(s, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
    perror("simplex-talk: connect");
    close(s);
    exit(EXIT_FAILURE);
  }

  // Main loop; get and send lines of text
  while (fgets(buf, sizeof(buf), stdin)) {
    buf[MAX_LINE - 1] = '\0'; // Null-terminate buffer
    len = strlen(buf) + 1;

    // Send the message
    send(s, buf, len, 0);
    printf("Sent out: %s", buf);

    // Receive the reply
    sleep(1);
    len = recv(s, buf, sizeof(buf), 0);
    printf("Received reply: %s", buf);
  }

  return EXIT_SUCCESS;
}
