CC=g++
CFLAGS = -Wall -Wextra -pedantic
LOGIN = xzaklo00
FILES = Makefile rdtclient.cpp rdtserver.cpp readme.xml udt.h
demo=udtdemo
proxy=udtproxy

all: rdtclient rdtserver

rdtclient: rdtclient.cpp udt.h
	$(CC) $(CFLAGS) -o $@ rdtclient.cpp

rdtserver: rdtserver.cpp udt.h
	$(CC) $(CFLAGS) -o $@  rdtserver.cpp

tar: 	clean
	tar -cf $(LOGIN).tar $(FILES)
	gzip $(LOGIN).tar

clean:
	rm -f *.o rdtclient rdtserver *~ *.tar.gz
