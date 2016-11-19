#ifndef CLIENT_COMMANDS_H
#define CLIENT_COMMANDS_H


#include "shell.h"
#include "dns.h"


#define N_COMMANDS 3


// Lookup commands. There are only 2: lookup and reverse lookup.
extern command_pair dns_lookup_commands[N_COMMANDS];


// The file-descriptor for the socket to send requests through.
extern int dns_server_fd;


// Send a lookup request.
void send_lookup_req(size_t argc, char** argv);


// Send a reverse lookup request.
void send_rev_lookup_req(size_t argc, char** argv);


// Send a malformed request (bogus packet)
void send_malformed_req(size_t argc, char** argv);


#endif // CLIENT_COMMANDS_H
