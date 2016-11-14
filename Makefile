CC = clang-3.7
CCFLAGS = -Wall -g
DEBUGFLAGS = -DDEBUG
TESTS = test_list test_hash_table

all: server client

tests: $(TESTS)

server: server.o dns.o lookup_table.o hash_table.o list.o SuperFastHash.o
	$(CC) $(CCFLAGS) -o server server.o dns.o lookup_table.o hash_table.o list.o \
          SuperFastHash.o

client: client.o dns.o lookup_table.o hash_table.o list.o SuperFastHash.o
	$(CC) $(CCFLAGS) -o client client.o dns.o lookup_table.o hash_table.o list.o \
          SuperFastHash.o

server.o: server.c dns.h
	$(CC) $(CCFLAGS) -c server.c

client.o: client.c dns.h
	$(CC) $(CCFLAGS) -c client.c

dns.o: dns.c dns.h
	$(CC) $(CCFLAGS) -c dns.c

list.o: list.c list.h
	$(CC) $(CCFLAGS) -c list.c

SuperFastHash.o: SuperFastHash.c SuperFastHash.h
	$(CC) $(CCFLAGS) -c SuperFastHash.c

hash_table.o: hash_table.c hash_table.h
	$(CC) $(CCFLAGS) -c hash_table.c

lookup_table.o: lookup_table.c lookup_table.h hash_table.h list.h
	$(CC) $(CCFLAGS) -c lookup_table.c

test_hash_table: test_hash_table.o hash_table.o SuperFastHash.o list.o
	$(CC) $(CCFLAGS) -o test_hash_table test_hash_table.o hash_table.o list.o

test_hash_table.o: test_hash_table.c test.h SuperFastHash.h
	$(CC) $(CCFLAGS) -c test_hash_table.c

test_list.o: test_list.c test.h
	$(CC) $(CCFLAGS) -c test_list.c

test_list: test_list.o list.o
	$(CC) $(CCFLAGS) -o test_list test_list.o list.o

debug: server.c client.c debug.h
	$(CC) $(CCFLAGS) $(DEBUGFLAGS) -o server server.c
	$(CC) $(CCFLAGS) $(DEBUGFLAGS) -o client client.c

clean:
	rm -rf server client *.o $(TESTS)

new:
	make clean
	make

.PHONY: all clean new
