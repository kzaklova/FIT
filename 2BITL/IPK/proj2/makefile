CC = g++
CFLAGS = -Wall -Wextra -pedantic
LOGIN = xzaklo00
FILES = makefile client.cpp server.cpp protokol.pdf

all: server client

server: server.cpp
	$(CC) $(CFLAGS) -o $@ server.cpp

client: client.cpp
	$(CC) $(CFLAGS) -o $@ client.cpp

clean:
	rm -f *.o *.out client server *.tar.gz *~

tar: 	clean
	tar -cf $(LOGIN).tar $(FILES)
	gzip $(LOGIN).tar
