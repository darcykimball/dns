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
  struct sockaddr_in src_addr; // To store address of a sender
  char buf[MAX_LINE]; // TODO
  socklen_t len; // TODO
  socklen_t src_addr_len;
  int s; // TODO
  struct msghdr msg; // Reply message to send
  struct iovec iov; // Singleton iovec for reply data

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

  // Wait for connection, then receive and print text
  while ((len = recvfrom(s, buf, sizeof(buf), 0,
                  (struct sockaddr*)&src_addr, &src_addr_len))) {
    printf("Received \"%s\"\n", buf);

    // Build reply message
    memset(&msg, 0, sizeof(msg));
    
    iov.iov_base = buf;
    iov.iov_len = len;

    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    msg.msg_name = &src_addr;
    msg.msg_namelen = sizeof(src_addr);

    sendmsg(s, &msg, 0); 
    printf("Sent back \"%s\"\n", buf);
  }

  return EXIT_SUCCESS;
}
