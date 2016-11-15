CC = clang-3.7
CCFLAGS = -Wall -g
DEBUGFLAGS = -DDEBUG
TESTS = test_list test_dns_packet

all: server client

tests: $(TESTS)

server: server.o dns.o dns_lookup_table.o list.o bimap.o
	$(CC) $(CCFLAGS) -o server server.o dns.o dns_lookup_table.o list.o bimap.o
         
client: client.o dns.o dns_lookup_table.h list.o shell.o client_commands.o
	$(CC) $(CCFLAGS) -o client client.o dns.o list.o shell.o client_commands.o

server.o: server.c dns.h
	$(CC) $(CCFLAGS) -c server.c

client.o: client.c dns.h
	$(CC) $(CCFLAGS) -c client.c

dns.o: dns.c dns.h
	$(CC) $(CCFLAGS) -c dns.c

list.o: list.c list.h
	$(CC) $(CCFLAGS) -c list.c

shell.o: shell.c shell.h
	$(CC) $(CCFLAGS) -c shell.c

client_commands.o: client_commands.c client_commands.h
	$(CC) $(CCFLAGS) -c client_commands.c

bimap.o: bimap.c bimap.h
	$(CC) $(CCFLAGS) -c bimap.c

dns_lookup_table.o: dns_lookup_table.c dns_lookup_table.h hash_table.h list.h
	$(CC) $(CCFLAGS) -c dns_lookup_table.c

test_list.o: test_list.c test.h
	$(CC) $(CCFLAGS) -c test_list.c

test_list: test_list.o list.o
	$(CC) $(CCFLAGS) -o test_list test_list.o list.o

test_dns_packet.o: test_dns_packet.c
	$(CC) $(CCFLAGS) -c test_dns_packet.c

test_dns_packet: test_dns_packet.o dns.o
	$(CC) $(CCFLAGS) -o test_dns_packet test_dns_packet.o dns.o

debug: server.c client.c debug.h
	$(CC) $(CCFLAGS) $(DEBUGFLAGS) -o server server.c
	$(CC) $(CCFLAGS) $(DEBUGFLAGS) -o client client.c

clean:
	rm -rf server client *.o $(TESTS)

new:
	make clean
	make

.PHONY: all clean new
