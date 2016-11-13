CC = clang-3.7
CCFLAGS = -Wall
DEBUGFLAGS = -DDEBUG

all: server client

server.o: server.c dns.h
	$(CC) $(CCFLAGS) -c server.c

client.o: client.c dns.h
	$(CC) $(CCFLAGS) -c client.c

dns.o: dns.c dns.h
	$(CC) $(CCFLAGS) -c dns.o

debug: server.c client.c debug.h
	$(CC) $(CCFLAGS) $(DEBUGFLAGS) -o server server.c
	$(CC) $(CCFLAGS) $(DEBUGFLAGS) -o client client.c

clean:
	rm -rf server client *.o

new:
	make clean
	make

.PHONY: all clean new
