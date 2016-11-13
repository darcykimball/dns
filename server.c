#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "debug.h"

#define SERVER_PORT 5432 
#define MAX_PENDING 5
#define MAX_LINE 256

int main() {
  struct sockaddr_in sin; // TODO
  char buf[MAX_LINE]; // TODO
  socklen_t len; // TODO
  int s, new_s; // TODO

  // Build address data structure
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons(SERVER_PORT);

  // Setup passive open
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("simplex-talk: socket");
    exit(EXIT_FAILURE);
  }

  if ((bind(s, (struct sockaddr*)&sin, sizeof(sin))) < 0) {
    perror("simplex-talk: bind");
    exit(EXIT_FAILURE);
  }

  listen(s, MAX_PENDING);

  // Wait for connection, then receive and print text
  while (1) {
    if ((new_s = accept(s, (struct sockaddr*)&sin, &len)) < 0) { // CHECK len!!
      perror("simplex-talk: accept");
      exit(EXIT_FAILURE);
    }

    while ((len = recv(new_s, buf, sizeof(buf), 0))) {
      fputs(buf, stdout);
    }

    close(new_s);
  }

  return EXIT_SUCCESS;
}
